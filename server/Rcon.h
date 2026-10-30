#ifndef RCON_H
#define RCON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Classes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/* Public constants                                                    */
/* ------------------------------------------------------------------ */
#define REFRESHX_HEADER_CHARS 10
#define RCON_AUTH_TIMEOUT_MS 5000

/* ------------------------------------------------------------------ */
/* Opaque forward declarations                                         */
/* ------------------------------------------------------------------ */
struct AdminServer;
struct AdminConnection;

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

/* Initialize the RCON subsystem with the admin password.
 * Returns a pointer to the AdminServer instance, or NULL on failure. */
struct AdminServer *Rcon_Init(const char *password);

/* Start listening for incoming admin connections (spawns internal thread). */
bool Rcon_Start(struct AdminServer *server);

/* Stop the admin server and wait for the listening thread to finish. */
void Rcon_Stop(struct AdminServer *server);

/* Free all resources allocated by Rcon_Init. */
void Rcon_Free(struct AdminServer *server);

/* Broadcast a text line to all authenticated admin clients. */
void Rcon_Broadcast(struct AdminServer *server, const char *text);

/* Send a text line to a specific admin client identified by IP address. */
void Rcon_SendToIP(struct AdminServer *server, const char *ip, const char *text);

/* Process pending admin commands (should be called regularly from the main loop). */
void Rcon_ProcessCommands(struct AdminServer *server);

/* ------------------------------------------------------------------ */
/* Internal structures (exposed only for implementation)               */
/* ------------------------------------------------------------------ */
struct AdminMessage {
    char *msg;                 /* owned, must be freed */
    struct AdminConnection *socket; /* back‑reference to originating connection */
};

struct AdminConnection {
    int sockfd;                /* connected socket */
    struct sockaddr_in addr;   /* peer address */
    bool authed;               /* authentication state */
    uint64_t connect_time_ms;  /* time of connection (ms) */
    struct AdminServer *server;/* back‑reference */
    struct Thread *thread;     /* handling thread */
};

struct AdminServer {
    char *password;            /* admin password (copied) */
    struct Thread *listen_thread;
    int listen_sock;           /* listening socket */
    int max_connections;       /* from net_maxadminconnections cvar */
    /* token bucket for rate limiting */
    unsigned int bucket_burst;
    unsigned int bucket_rate;  /* tokens per second */
    unsigned int bucket_tokens;
    time_t bucket_last_fill;   /* seconds since epoch */
    /* thread‑safe lists */
    struct CriticalSection *admins_cs;
    struct AdminConnection **admins;   /* dynamic array */
    int admins_count;
    int admins_capacity;
    struct CriticalSection *msgq_cs;
    struct AdminMessage **msg_queue;   /* dynamic array */
    int msgq_count;
    int msgq_capacity;
    bool terminated;
};

#ifdef __cplusplus
}
#endif

#endif /* RCON_H */