/* ClientLauncherIPC — C port of client/ClientLauncherIPC.pas */

#ifndef CLIENTLAUNCHERIPC_H
#define CLIENTLAUNCHERIPC_H

#include "LauncherIPC.h"

struct ClientLauncherIPC;

/* Create a new client launcher IPC */
struct ClientLauncherIPC *ClientLauncherIPC_Create(void);

/* Destroy client launcher IPC */
void ClientLauncherIPC_Free(struct ClientLauncherIPC *ipc);

#endif /* CLIENTLAUNCHERIPC_H */