/* LauncherConnection — C port of shared/LauncherConnection.pas */

#ifndef LAUNCHERCONNECTION_H
#define LAUNCHERCONNECTION_H

#include <stdbool.h>
#include <stdint.h>

/* Callback type for received messages */
typedef void (*LauncherConnectionMessageCallback)(const char *message);

/* Opaque structure */
struct LauncherConnection;

/* Create a new launcher connection thread */
struct LauncherConnection *LauncherConnection_Create(int port);

/* Destroy launcher connection */
void LauncherConnection_Free(struct LauncherConnection *conn);

/* Send a message to the launcher */
void LauncherConnection_SendMessage(struct LauncherConnection *conn, const char *message);

/* Set the message callback (called from main thread) */
void LauncherConnection_SetMessageCallback(struct LauncherConnection *conn, LauncherConnectionMessageCallback callback);

#endif /* LAUNCHERCONNECTION_H */