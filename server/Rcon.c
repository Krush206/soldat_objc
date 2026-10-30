/* Rcon — C port of server/Rcon.pas */

#include "Rcon.h"
#include "SysUtils.h"
#include "SyncObjs.h"
#include "Classes.h"
#include "Net.h"
#include "Constants.h"
#include "Command.h"
#include "Console.h"
#include "TraceLog.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

/* Global admin server instance */
extern struct AdminServer *g_admin_server;

/* ------------------------------------------------------------------ */
/* External game globals (defined elsewhere)                           */
/* ------------------------------------------------------------------ */
extern int sv_maxplayers;
extern int sv_maxspectators;
extern int sv_timelimit;
extern int sv_killlimit;
extern int sv_gamemode;
extern char *sv_password;
extern char *MapName;
extern int TimeLimitCounter;
extern int TeamScore[5];
extern int TeamFlag[5];
extern struct TSprite *Sprite;          /* opaque */
extern int *SortedPlayers;              /* array of player indices */
extern char *DEDVERSION;

/* Script dispatcher callbacks (optional) */
#ifdef SCRIPT
extern void ScrptDispatcher_OnAdminMessage(const char *ip, unsigned short port, const char *msg);
extern void ScrptDispatcher_OnAdminConnect(const char *ip, unsigned short port);
extern void ScrptDispatcher_OnAdminDisconnect(const char *ip, unsigned short port);
extern bool ScrptDispatcher_OnConsoleCommand(const char *ip, unsigned short port, const char *cmd);
#endif

/* ------------------------------------------------------------------ */
/* Helper: dynamic array with critical section                         */
/* ------------------------------------------------------------------ */
static void list_init(void ***arr, int *count, int *capacity) {
    *arr = NULL;
    *count = 0;
    *capacity = 0;
}
static void list_free(void ***arr, int *count) {
    if (*arr) {
        free(*arr);
        *arr = NULL;
    }
    *count = 0;
}
static bool list_add(void ***arr, int *count, int *capacity, void *item) {
    if (*count >= *capacity) {
        int newcap = *capacity ? *capacity * 2 : 8;
        void **newarr = realloc(*arr, newcap * sizeof(void *));
        if (!newarr) return false;
        *arr = newarr;
        *capacity = newcap;
    }
    (*arr)[(*count)++] = item;
    return true;
}
static void list_remove(void ***arr, int *count, int index) {
    if (index < 0 || index >= *count) return;
    for (int i = index; i < *count - 1; i++)
        (*arr)[i] = (*arr)[i + 1];
    (*count)--;
}

/* ------------------------------------------------------------------ */
/* Socket utilities                                                    */
/* ------------------------------------------------------------------ */
static int socket_set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
static int socket_set_keepalive(int fd) {
    int yes = 1;
    return setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(yes));
}
static int socket_reuseaddr(int fd) {
    int yes = 1;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
}

/* ------------------------------------------------------------------ */
/* Token bucket rate limiting                                          */
/* ------------------------------------------------------------------ */
static bool token_bucket_consume(struct AdminServer *srv) {
    time_t now = time(NULL);
    if (now > srv->bucket_last_fill) {
        unsigned int elapsed = (unsigned int)(now - srv->bucket_last_fill);
        unsigned long long add = (unsigned long long)srv->bucket_rate * elapsed;
        if (add > srv->bucket_burst) add = srv->bucket_burst;
        srv->bucket_tokens += (unsigned int)add;
        if (srv->bucket_tokens > srv->bucket_burst) srv->bucket_tokens = srv->bucket_burst;
        srv->bucket_last_fill = now;
    }
    if (srv->bucket_tokens == 0) return false;
    srv->bucket_tokens--;
    return true;
}

/* ------------------------------------------------------------------ */
/* AdminConnection implementation                                      */
/* ------------------------------------------------------------------ */
static void adminconn_write_string(struct AdminConnection *c, const char *str) {
    char *buf = strconcat(str, "\r\n");
    if (!buf) return;
    ssize_t written = write(c->sockfd, buf, strlen(buf));
    if (written < 0) {
        TraceLog_Debug("[RCON] Error during sending message: %d", errno);
    }
    free(buf);
}

static void adminconn_shutdown(struct AdminConnection *c) {
    shutdown(c->sockfd, SHUT_RDWR);
}

/* Thread entry for a single admin connection */
static void *adminconn_thread_main(void *arg) {
    struct AdminConnection *c = (struct AdminConnection *)arg;
    char buffer[1024];
    char linebuf[4096];
    size_t line_len = 0;
    bool authed = false;
    uint64_t connect_time = c->connect_time_ms;

    if (!c->server->terminated)
        adminconn_write_string(c, "OpenSoldat Admin Connection Established.");

    while (!c->server->terminated) {
        if (!authed) {
            uint64_t now = (uint64_t)(time(NULL) * 1000);
            if (now - connect_time >= RCON_AUTH_TIMEOUT_MS) {
                TraceLog_Debug("[RCON] Password request timed out (%s).", inet_ntoa(c->addr.sin_addr));
                adminconn_write_string(c, "Password request timed out.");
                break;
            }
        }

        ssize_t n = read(c->sockfd, buffer, sizeof(buffer));
        if (n > 0) {
            for (ssize_t i = 0; i < n; i++) {
                char ch = buffer[i];
                if (ch == '\n') {
                    linebuf[line_len] = '\0';
                    line_len = 0;
                    /* process line */
                    if (authed) {
                        if (strlen(linebuf) > 0) {
                            struct AdminMessage *msg = calloc(1, sizeof(struct AdminMessage));
                            msg->msg = strdup(linebuf);
                            msg->socket = c;
                            CriticalSection_Acquire(c->server->msgq_cs);
                            list_add((void***)&c->server->msg_queue, &c->server->msgq_count, &c->server->msgq_capacity, msg);
                            CriticalSection_Release(c->server->msgq_cs);
                        }
                    } else {
                        if (strcmp(linebuf, c->server->password) == 0) {
                            authed = true;
                            c->authed = true;
                            TraceLog_Debug("[RCON] Admin connected (%s).", inet_ntoa(c->addr.sin_addr));
                            adminconn_write_string(c, "Welcome, you are in command of the server now.");
                            adminconn_write_string(c, "List of commands available in the OpenSoldat game Manual.");
                            adminconn_write_string(c, strconcat("Server Version: ", DEDVERSION));
#ifdef SCRIPT
                            ScrptDispatcher_OnAdminConnect(inet_ntoa(c->addr.sin_addr), ntohs(c->addr.sin_port));
#endif
                        } else {
                            adminconn_write_string(c, "Invalid password.");
                            TraceLog_Debug("[RCON] Invalid password from: %s", inet_ntoa(c->addr.sin_addr));
                            break;
                        }
                    }
                } else if (ch != '\r') {
                    if (line_len < sizeof(linebuf) - 1)
                        linebuf[line_len++] = ch;
                }
            }
        } else if (n == 0) {
            break; /* graceful close */
        } else {
            if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
                TraceLog_Debug("[RCON] Socket read failed, errno: %d for %s", errno, inet_ntoa(c->addr.sin_addr));
                break;
            }
        }
    }

    if (authed) {
        TraceLog_Debug("[RCON] Admin disconnected (%s).", inet_ntoa(c->addr.sin_addr));
#ifdef SCRIPT
        ScrptDispatcher_OnAdminDisconnect(inet_ntoa(c->addr.sin_addr), ntohs(c->addr.sin_port));
#endif
    }

    /* remove from admin list */
    CriticalSection_Acquire(c->server->admins_cs);
    for (int i = 0; i < c->server->admins_count; i++) {
        if (c->server->admins[i] == c) {
            list_remove((void***)&c->server->admins, &c->server->admins_count, i);
            break;
        }
    }
    CriticalSection_Release(c->server->admins_cs);

    close(c->sockfd);
    free(c);
    return NULL;
}

/* Forward declaration */
static void *adminserver_listen_thread(void *arg);

/* Wrapper for Thread API - takes Thread* and calls the actual listen function */
static void adminserver_listen_thread_wrapper(Thread *t) {
    (void)t;
    if (g_admin_server) {
        adminserver_listen_thread(g_admin_server);
    }
}

/* ------------------------------------------------------------------ */
/* AdminServer listen thread                                           */
/* ------------------------------------------------------------------ */
static void *adminserver_listen_thread(void *arg) {
    struct AdminServer *srv = (struct AdminServer *)arg;
    struct sockaddr_in cli_addr;
    socklen_t cli_len = sizeof(cli_addr);

    while (!srv->terminated) {
        int client_fd = accept(srv->listen_sock, (struct sockaddr *)&cli_addr, &cli_len);
        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                Sleep(10);
                continue;
            }
            if (srv->terminated) break;
            TraceLog_Debug("[RCON] accept error: %d", errno);
            continue;
        }

        /* rate limit */
        if (!token_bucket_consume(srv)) {
            TraceLog_Debug("[RCON] New connection rejected: Rate limit exceeded");
            close(client_fd);
            continue;
        }

        /* max connections */
        CriticalSection_Acquire(srv->admins_cs);
        bool allow = srv->admins_count < srv->max_connections;
        CriticalSection_Release(srv->admins_cs);
        if (!allow) {
            TraceLog_Debug("[RCON] New connection rejected: Maximum number of connections reached");
            close(client_fd);
            continue;
        }

        socket_set_nonblocking(client_fd);
        socket_set_keepalive(client_fd);

        struct AdminConnection *c = calloc(1, sizeof(struct AdminConnection));
        c->sockfd = client_fd;
        c->addr = cli_addr;
        c->authed = false;
        c->connect_time_ms = (uint64_t)(time(NULL) * 1000);
        c->server = srv;

        CriticalSection_Acquire(srv->admins_cs);
        list_add((void***)&srv->admins, &srv->admins_count, &srv->admins_capacity, c);
        CriticalSection_Release(srv->admins_cs);

        pthread_t pt;
        pthread_create(&pt, NULL, adminconn_thread_main, c);
        pthread_detach(pt);
    }
    return NULL;
}

/* ------------------------------------------------------------------ */
/* Console wrapper with varargs support                               */
/* ------------------------------------------------------------------ */
static void MainConsole_ConsoleV(const char *format, int color, va_list args) {
    extern TConsole MainConsole;
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    console_write(&MainConsole, buffer, (unsigned int)color);
}

void MainConsole_Console(const char *format, int color, ...) {
    va_list args;
    va_start(args, color);
    MainConsole_ConsoleV(format, color, args);
    va_end(args);
}

/* ------------------------------------------------------------------ */
/* Public API implementations                                          */
/* ------------------------------------------------------------------ */
struct AdminServer *Rcon_Init(const char *password) {
    struct AdminServer *srv = calloc(1, sizeof(struct AdminServer));
    if (!srv) return NULL;
    srv->password = strdup(password ? password : "");
    srv->max_connections = 10; /* default, will be overwritten by cvars later */
    srv->bucket_burst = 10;
    srv->bucket_rate = 5;
    srv->bucket_tokens = 0;
    srv->bucket_last_fill = time(NULL);
    srv->terminated = false;

    srv->admins_cs = CriticalSection_Create();
    srv->msgq_cs = CriticalSection_Create();
    list_init((void***)&srv->admins, &srv->admins_count, &srv->admins_capacity);
    list_init((void***)&srv->msg_queue, &srv->msgq_count, &srv->msgq_capacity);
    return srv;
}

bool Rcon_Start(struct AdminServer *srv) {
    if (!srv) return false;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return false;
    socket_set_nonblocking(fd);
    socket_set_keepalive(fd);
    socket_reuseaddr(fd);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; /* net_adminip cvar would set specific IP */
    addr.sin_port = htons(23073);       /* net_port cvar */

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(fd);
        return false;
    }
    if (listen(fd, 5) < 0) {
        close(fd);
        return false;
    }
    srv->listen_sock = fd;

    g_admin_server = srv;

    srv->listen_thread = Thread_Create();
    if (!Thread_Start(srv->listen_thread, (ThreadExecuteFunc)adminserver_listen_thread_wrapper)) {
        Thread_Free(srv->listen_thread);
        srv->listen_thread = NULL;
        close(fd);
        return false;
    }
    return true;
}

void Rcon_Stop(struct AdminServer *srv) {
    if (!srv) return;
    srv->terminated = true;
    if (srv->listen_sock >= 0) {
        shutdown(srv->listen_sock, SHUT_RDWR);
        close(srv->listen_sock);
        srv->listen_sock = -1;
    }
    if (srv->listen_thread) {
        Thread_WaitFor(srv->listen_thread);
        Thread_Free(srv->listen_thread);
        srv->listen_thread = NULL;
    }
    /* close all admin connections */
    CriticalSection_Acquire(srv->admins_cs);
    for (int i = 0; i < srv->admins_count; i++) {
        struct AdminConnection *c = srv->admins[i];
        adminconn_shutdown(c);
    }
    CriticalSection_Release(srv->admins_cs);
    /* wait a bit for threads to exit */
    Sleep(100);
}

void Rcon_Free(struct AdminServer *srv) {
    if (!srv) return;
    Rcon_Stop(srv);
    CriticalSection_Acquire(srv->admins_cs);
    for (int i = 0; i < srv->admins_count; i++) {
        free(srv->admins[i]); /* connections already closed */
    }
    list_free((void***)&srv->admins, &srv->admins_count);
    CriticalSection_Release(srv->admins_cs);
    CriticalSection_Free(srv->admins_cs);

    CriticalSection_Acquire(srv->msgq_cs);
    for (int i = 0; i < srv->msgq_count; i++) {
        struct AdminMessage *m = srv->msg_queue[i];
        free(m->msg);
        free(m);
    }
    list_free((void***)&srv->msg_queue, &srv->msgq_count);
    CriticalSection_Release(srv->msgq_cs);
    CriticalSection_Free(srv->msgq_cs);

    free(srv->password);
    free(srv);
}

/* ------------------------------------------------------------------ */
/* Broadcast / SendToIP                                               */
/* ------------------------------------------------------------------ */
void Rcon_Broadcast(struct AdminServer *srv, const char *text) {
    if (!srv || !text) return;
    CriticalSection_Acquire(srv->admins_cs);
    for (int i = 0; i < srv->admins_count; i++) {
        struct AdminConnection *c = srv->admins[i];
        if (c->authed) adminconn_write_string(c, text);
    }
    CriticalSection_Release(srv->admins_cs);
}

void Rcon_SendToIP(struct AdminServer *srv, const char *ip, const char *text) {
    if (!srv || !ip || !text) return;
    CriticalSection_Acquire(srv->admins_cs);
    for (int i = 0; i < srv->admins_count; i++) {
        struct AdminConnection *c = srv->admins[i];
        if (c->authed && strcmp(inet_ntoa(c->addr.sin_addr), ip) == 0)
            adminconn_write_string(c, text);
    }
    CriticalSection_Release(srv->admins_cs);
}

/* ------------------------------------------------------------------ */
/* ProcessCommands – called from main loop                             */
/* ------------------------------------------------------------------ */
void Rcon_ProcessCommands(struct AdminServer *srv) {
    if (!srv) return;
    CriticalSection_Acquire(srv->msgq_cs);
    if (srv->msgq_count == 0) {
        CriticalSection_Release(srv->msgq_cs);
        return;
    }
    struct AdminMessage *msg = srv->msg_queue[0];
    list_remove((void***)&srv->msg_queue, &srv->msgq_count, 0);
    CriticalSection_Release(srv->msgq_cs);

    if (!msg) return;

    if (strcmp(msg->msg, "SHUTDOWN") == 0) {
        MainConsole_Console("[RCON] SHUTDOWN (%s).", GAME_MESSAGE_COLOR,
                            inet_ntoa(msg->socket->addr.sin_addr));
        /* signal main loop to exit – external global */
        extern bool ProgReady;
        ProgReady = false;
        free(msg->msg);
        free(msg);
        return;
    }

    if (strcmp(msg->msg, "REFRESHX") == 0) {
        /* Build REFRESHX packet – placeholder using game globals */
        /* For brevity we only send a minimal header */
        char header[REFRESHX_HEADER_CHARS + 1];
        snprintf(header, sizeof(header), "REFRESHX\r\n");
        write(msg->socket->sockfd, header, strlen(header));
        free(msg->msg);
        free(msg);
        return;
    }

    /* Normal admin command */
    const char *ip = inet_ntoa(msg->socket->addr.sin_addr);
    unsigned short port = ntohs(msg->socket->addr.sin_port);
    MainConsole_Console("%s (%s)", GAME_MESSAGE_COLOR, msg->msg, ip);
#ifdef SCRIPT
    ScrptDispatcher_OnAdminMessage(ip, port, msg->msg);
#endif
    if (msg->msg[0] == '/') {
#ifdef SCRIPT
        if (!ScrptDispatcher_OnConsoleCommand(ip, port, msg->msg)) {
#endif
            /* strip leading '/' and parse */
            char *cmd = strdup(msg->msg + 1);
            Command_ParseInput(cmd, 255);
            free(cmd);
#ifdef SCRIPT
        }
#endif
    }
    free(msg->msg);
    free(msg);
}

/* ------------------------------------------------------------------ */
/* Global wrappers used by legacy code                                 */
/* ------------------------------------------------------------------ */
struct AdminServer *g_admin_server = NULL;

void BroadcastMsg(const char *text) {
    if (g_admin_server) Rcon_Broadcast(g_admin_server, text);
}
void SendMessageToAdmin(const char *toIP, const char *text) {
    if (g_admin_server) Rcon_SendToIP(g_admin_server, toIP, text);
}

/* Initialize global admin server (called from server startup) */
bool Rcon_GlobalInit(const char *password) {
    g_admin_server = Rcon_Init(password);
    if (!g_admin_server) return false;
    return Rcon_Start(g_admin_server);
}
void Rcon_GlobalShutdown(void) {
    if (g_admin_server) {
        Rcon_Stop(g_admin_server);
        Rcon_Free(g_admin_server);
        g_admin_server = NULL;
    }
}