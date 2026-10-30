/* LauncherConnection — C port of shared/LauncherConnection.pas */

#include "LauncherConnection.h"
#include "TraceLog.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_MESSAGE_LENGTH 4096
#define CONNECT_TIMEOUT_MS 5000
#define IO_TIMEOUT_MS 1000
#define SLEEP_MS 250

struct LauncherConnection {
    int port;
    int socket_fd;
    pthread_t thread;
    pthread_mutex_t send_queue_mutex;
    char **send_queue;
    int send_queue_size;
    int send_queue_capacity;
    bool terminated;
    char *received_message;
    LauncherConnectionMessageCallback message_callback;
    bool connected;
};

/* Thread function */
static void *LauncherConnection_ThreadFunc(void *arg)
{
    struct LauncherConnection *conn = (struct LauncherConnection *)arg;
    char read_buffer[MAX_MESSAGE_LENGTH];
    int read_length;
    fd_set read_fds;
    struct timeval timeout;

    /* Create socket */
    conn->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (conn->socket_fd < 0) {
        TraceLog_Debug("[LauncherConnection] Failed to create socket");
        return NULL;
    }

    /* Set non-blocking */
    int flags = fcntl(conn->socket_fd, F_GETFL, 0);
    fcntl(conn->socket_fd, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(conn->port);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    /* Connect with timeout */
    int connect_result = connect(conn->socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (connect_result < 0 && errno != EINPROGRESS) {
        TraceLog_Debug("[LauncherConnection] Connection failed: %s", strerror(errno));
        close(conn->socket_fd);
        conn->socket_fd = -1;
        return NULL;
    }

    if (errno == EINPROGRESS) {
        FD_ZERO(&read_fds);
        FD_SET(conn->socket_fd, &read_fds);
        timeout.tv_sec = CONNECT_TIMEOUT_MS / 1000;
        timeout.tv_usec = (CONNECT_TIMEOUT_MS % 1000) * 1000;

        int select_result = select(conn->socket_fd + 1, NULL, &read_fds, NULL, &timeout);
        if (select_result <= 0) {
            TraceLog_Debug("[LauncherConnection] Connection timeout");
            close(conn->socket_fd);
            conn->socket_fd = -1;
            return NULL;
        }

        int so_error;
        socklen_t len = sizeof(so_error);
        getsockopt(conn->socket_fd, SOL_SOCKET, SO_ERROR, &so_error, &len);
        if (so_error != 0) {
            TraceLog_Debug("[LauncherConnection] Connection failed: %s", strerror(so_error));
            close(conn->socket_fd);
            conn->socket_fd = -1;
            return NULL;
        }
    }

    /* Set blocking mode for read/write */
    fcntl(conn->socket_fd, F_SETFL, flags & ~O_NONBLOCK);

    TraceLog_Debug("[LauncherConnection] Connected");
    conn->connected = true;

    while (!conn->terminated) {
        FD_ZERO(&read_fds);
        FD_SET(conn->socket_fd, &read_fds);
        timeout.tv_sec = IO_TIMEOUT_MS / 1000;
        timeout.tv_usec = (IO_TIMEOUT_MS % 1000) * 1000;

        int select_result = select(conn->socket_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (select_result > 0 && FD_ISSET(conn->socket_fd, &read_fds)) {
            read_length = recv(conn->socket_fd, read_buffer, MAX_MESSAGE_LENGTH - 1, 0);
            if (read_length > 0) {
                read_buffer[read_length] = '\0';
                free(conn->received_message);
                conn->received_message = strdup(read_buffer);

                if (conn->message_callback) {
                    conn->message_callback(conn->received_message);
                }
            } else if (read_length == 0) {
                TraceLog_Debug("[LauncherConnection] Launcher closed the connection");
                break;
            } else {
                if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) {
                    TraceLog_Debug("[LauncherConnection] Failed to read from socket. Error: %s", strerror(errno));
                    break;
                }
            }
        }

        /* Process send queue */
        pthread_mutex_lock(&conn->send_queue_mutex);
        if (conn->send_queue_size > 0 && conn->socket_fd >= 0) {
            char *message = conn->send_queue[0];
            int len = strlen(message);
            int sent = send(conn->socket_fd, message, len, 0);
            if (sent < 0) {
                TraceLog_Debug("[LauncherConnection] Failed to write to socket. Error: %s", strerror(errno));
            } else {
                /* Remove from queue */
                free(conn->send_queue[0]);
                for (int i = 1; i < conn->send_queue_size; i++) {
                    conn->send_queue[i - 1] = conn->send_queue[i];
                }
                conn->send_queue_size--;
            }
        }
        pthread_mutex_unlock(&conn->send_queue_mutex);

        usleep(SLEEP_MS * 1000);
    }

    if (conn->socket_fd >= 0) {
        close(conn->socket_fd);
        conn->socket_fd = -1;
    }

    return NULL;
}

struct LauncherConnection *LauncherConnection_Create(int port)
{
    struct LauncherConnection *conn = calloc(1, sizeof(struct LauncherConnection));
    if (!conn) return NULL;

    conn->port = port;
    conn->socket_fd = -1;
    conn->terminated = false;
    conn->send_queue_capacity = 16;
    conn->send_queue = calloc(conn->send_queue_capacity, sizeof(char *));
    pthread_mutex_init(&conn->send_queue_mutex, NULL);

    int result = pthread_create(&conn->thread, NULL, LauncherConnection_ThreadFunc, conn);
    if (result != 0) {
        TraceLog_Debug("[LauncherConnection] Failed to create thread");
        free(conn->send_queue);
        pthread_mutex_destroy(&conn->send_queue_mutex);
        free(conn);
        return NULL;
    }

    return conn;
}

void LauncherConnection_Free(struct LauncherConnection *conn)
{
    if (!conn) return;

    conn->terminated = true;
    pthread_join(conn->thread, NULL);

    if (conn->socket_fd >= 0) {
        close(conn->socket_fd);
    }

    free(conn->received_message);

    pthread_mutex_lock(&conn->send_queue_mutex);
    for (int i = 0; i < conn->send_queue_size; i++) {
        free(conn->send_queue[i]);
    }
    free(conn->send_queue);
    pthread_mutex_unlock(&conn->send_queue_mutex);

    pthread_mutex_destroy(&conn->send_queue_mutex);
    free(conn);
}

void LauncherConnection_SendMessage(struct LauncherConnection *conn, const char *message)
{
    if (!conn || !message) return;

    TraceLog_Debug("[LauncherConnection] Sending %s", message);

    pthread_mutex_lock(&conn->send_queue_mutex);

    if (conn->send_queue_size >= conn->send_queue_capacity) {
        conn->send_queue_capacity *= 2;
        conn->send_queue = realloc(conn->send_queue, conn->send_queue_capacity * sizeof(char *));
    }

    conn->send_queue[conn->send_queue_size] = strdup(message);
    conn->send_queue_size++;

    pthread_mutex_unlock(&conn->send_queue_mutex);
}

void LauncherConnection_SetMessageCallback(struct LauncherConnection *conn, LauncherConnectionMessageCallback callback)
{
    if (conn) {
        conn->message_callback = callback;
    }
}