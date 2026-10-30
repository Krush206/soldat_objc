/* ServerLauncherIPC — C port of server/ServerLauncherIPC.pas */

#ifndef SERVERLAUNCHERIPC_H
#define SERVERLAUNCHERIPC_H

#include "LauncherIPC.h"

struct ServerLauncherIPC;

/* Create a new server launcher IPC */
struct ServerLauncherIPC *ServerLauncherIPC_Create(void);

/* Destroy server launcher IPC */
void ServerLauncherIPC_Free(struct ServerLauncherIPC *ipc);

#endif /* SERVERLAUNCHERIPC_H */