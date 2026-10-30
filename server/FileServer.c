/* FileServer.c – minimal HTTP file server for OpenSoldat
 *
 * Implements the logic from FileServer.pas:
 *   * Starts a TCP listener on the configured address/port.
 *   * Accepts connections up to fileserver_maxconnections.
 *   * Handles simple GET requests for /mods/*.smod and /maps/*.smap.
 *   * Serves the requested file from UserDirectory.
 *
 * This implementation uses only POSIX APIs and follows the existing
 * coding style of the project (see Server.h, shared/LogFile.h, etc.).
 */

#include "FileServer.h"
#include "Server.h"               /* for cvars and UserDirectory */
#include "Version.h"              /* OPENSOLDAT_VERSION */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <limits.h>

/* ------------------------------------------------------------------ */
/* Internal state                                                     */
/* ------------------------------------------------------------------ */
static int listen_fd = -1;
static pthread_t server_thread;
static volatile int server_running = 0;

/* ------------------------------------------------------------------ */
/* Helper: send a full buffer on a socket                             */
/* ------------------------------------------------------------------ */
static int send_all(int fd, const void *buf, size_t len)
{
    const char *p = (const char *)buf;
    while (len > 0) {
        ssize_t n = send(fd, p, len, 0);
        if (n <= 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        p += n;
        len -= n;
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* Helper: build and send an HTTP response header                     */
/* ------------------------------------------------------------------ */
static void send_response_header(int fd, int status_code,
                                 const char *status_text,
                                 const char *content_type,
                                 long long content_length,
                                 const char *extra_headers)
{
    char header[1024];
    int n = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %lld\r\n"
        "Connection: close\r\n"
        "%s\r\n",
        status_code, status_text,
        content_type,
        content_length,
        extra_headers ? extra_headers : "");
    if (n > 0 && n < (int)sizeof(header))
        send_all(fd, header, n);
}

/* ------------------------------------------------------------------ */
/* Serve a single file                                                */
/* ------------------------------------------------------------------ */
static void serve_file(int client_fd, const char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        send_response_header(client_fd, 404, "Not Found",
                             "text/plain", 13, NULL);
        send_all(client_fd, "404 Not Found", 13);
        return;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        close(fd);
        send_response_header(client_fd, 500, "Internal Server Error",
                             "text/plain", 21, NULL);
        send_all(client_fd, "500 Internal Error", 21);
        return;
    }

    const char *filename = strrchr(path, '/');
    filename = filename ? filename + 1 : path;

    char extra[256];
    snprintf(extra, sizeof(extra),
        "Content-Disposition: attachment; filename=\"%s\"", filename);

    send_response_header(client_fd, 200, "OK",
                         "application/octet-stream",
                         st.st_size, extra);

    char buf[8192];
    ssize_t r;
    while ((r = read(fd, buf, sizeof(buf))) > 0) {
        if (send_all(client_fd, buf, r) < 0) break;
    }
    close(fd);
}

/* ------------------------------------------------------------------ */
/* Process a single HTTP request                                      */
/* ------------------------------------------------------------------ */
static void handle_client(int client_fd)
{
    char request[4096];
    ssize_t n = recv(client_fd, request, sizeof(request) - 1, 0);
    if (n <= 0) {
        close(client_fd);
        return;
    }
    request[n] = '\0';

    /* Very small HTTP parser – we only care about the first line */
    char method[16], uri[1024], version[16];
    if (sscanf(request, "%15s %1023s %15s", method, uri, version) != 3) {
        send_response_header(client_fd, 400, "Bad Request",
                             "text/plain", 11, NULL);
        send_all(client_fd, "400 Bad Request", 15);
        close(client_fd);
        return;
    }

    /* Only GET is supported */
    if (strcmp(method, "GET") != 0) {
        send_response_header(client_fd, 405, "Method Not Allowed",
                             "text/plain", 18, NULL);
        send_all(client_fd, "405 Method Not Allowed", 22);
        close(client_fd);
        return;
    }

    /* Optional User-Agent check – mimic original behaviour */
    const char *ua_ptr = strstr(request, "User-Agent:");
    if (ua_ptr) {
        const char *expected = "opensoldatclient/" OPENSOLDAT_VERSION;
        if (strncmp(ua_ptr + 11, expected, strlen(expected)) != 0) {
            send_response_header(client_fd, 403, "Forbidden",
                                 "text/plain", 9, NULL);
            send_all(client_fd, "403 Forbidden", 13);
            close(client_fd);
            return;
        }
    }

    /* Accept only /mods/*.smod or /maps/*.smap */
    int allowed = 0;
    if (strncmp(uri, "/mods/", 6) == 0 &&
        strlen(uri) > 6 && strcmp(uri + strlen(uri) - 5, ".smod") == 0)
        allowed = 1;
    else if (strncmp(uri, "/maps/", 6) == 0 &&
        strlen(uri) > 6 && strcmp(uri + strlen(uri) - 5, ".smap") == 0)
        allowed = 1;

    if (!allowed) {
        send_response_header(client_fd, 403, "Forbidden",
                             "text/plain", 9, NULL);
        send_all(client_fd, "403 Forbidden", 13);
        close(client_fd);
        return;
    }

    /* Build filesystem path safely */
    char fspath[PATH_MAX];
    snprintf(fspath, sizeof(fspath), "%s%s", UserDirectory, uri);

    /* Prevent directory traversal */
    char real_user[PATH_MAX], real_file[PATH_MAX];
    if (!realpath(UserDirectory, real_user) ||
        !realpath(fspath, real_file) ||
        strncmp(real_file, real_user, strlen(real_user)) != 0) {
        send_response_header(client_fd, 403, "Forbidden",
                             "text/plain", 9, NULL);
        send_all(client_fd, "403 Forbidden", 13);
        close(client_fd);
        return;
    }

    serve_file(client_fd, real_file);
    close(client_fd);
}

/* ------------------------------------------------------------------ */
/* Server thread main loop                                            */
/* ------------------------------------------------------------------ */
static void *server_loop(void *arg)
{
    (void)arg;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (server_running) {
        int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            if (errno == EINTR || errno == EAGAIN) continue;
            if (!server_running) break;   /* shutting down */
            perror("[FileServer] accept");
            continue;
        }

        /* Simple concurrency limit */
        /* (the original Pascal code used ConnectionCount) */
        /* Here we just handle sequentially – good enough for file transfers. */
        handle_client(client_fd);
    }
    return NULL;
}

/* ------------------------------------------------------------------ */
/* Public API                                                         */
/* ------------------------------------------------------------------ */
void StartFileServer(void)
{
    if (server_running) return;
    if (!fileserver_enable.value) return;   /* cvar may be disabled */

    const char *ip = fileserver_ip.value && fileserver_ip.value[0] ? fileserver_ip.value : "0.0.0.0";
    int port = fileserver_port.value ? fileserver_port.value : (net_port.value + 10);
    int maxconn = fileserver_maxconnections.value > 0 ? fileserver_maxconnections.value : 10;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("[FileServer] socket");
        return;
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        fprintf(stderr, "[FileServer] invalid IP address: %s\n", ip);
        close(listen_fd);
        listen_fd = -1;
        return;
    }

    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("[FileServer] bind");
        close(listen_fd);
        listen_fd = -1;
        return;
    }

    if (listen(listen_fd, maxconn) < 0) {
        perror("[FileServer] listen");
        close(listen_fd);
        listen_fd = -1;
        return;
    }

    server_running = 1;
    if (pthread_create(&server_thread, NULL, server_loop, NULL) != 0) {
        perror("[FileServer] pthread_create");
        server_running = 0;
        close(listen_fd);
        listen_fd = -1;
        return;
    }

    printf("[FileServer] Starting fileserver on %s:%d\n", ip, port);
}

void StopFileServer(void)
{
    if (!server_running) return;

    server_running = 0;
    if (listen_fd >= 0) {
        shutdown(listen_fd, SHUT_RDWR);
        close(listen_fd);
        listen_fd = -1;
    }
    pthread_join(server_thread, NULL);
    printf("[FileServer] Stopping fileserver\n");
}