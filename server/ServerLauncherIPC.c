/* ServerLauncherIPC — C port of server/ServerLauncherIPC.pas */

#include "ServerLauncherIPC.h"
#include "LauncherIPC.h"

#include <stdlib.h>

struct ServerLauncherIPC {
    struct LauncherIPC base;
};

static void ServerLauncherIPC_HandleMessage(struct LauncherIPC *ipc, const char *message)
{
    (void)ipc;
    if (message) {
        LauncherIPC_HandleCommand(ipc, message);
    }
}

struct ServerLauncherIPC *ServerLauncherIPC_Create(void)
{
    struct ServerLauncherIPC *ipc = calloc(1, sizeof(struct ServerLauncherIPC));
    if (!ipc) return NULL;

    ipc->base.handle_message = ServerLauncherIPC_HandleMessage;
    ipc->base.handle_command = LauncherIPC_HandleCommand;

    return ipc;
}

void ServerLauncherIPC_Free(struct ServerLauncherIPC *ipc)
{
    if (!ipc) return;

    LauncherIPC_Free(&ipc->base);
    free(ipc);
}