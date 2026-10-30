/* ClientLauncherIPC — C port of client/ClientLauncherIPC.pas */

#include "ClientLauncherIPC.h"
#include "LauncherIPC.h"

#include <stdlib.h>

struct ClientLauncherIPC {
    struct LauncherIPC base;
};

static void ClientLauncherIPC_HandleMessage(struct LauncherIPC *ipc, const char *message)
{
    (void)ipc;
    if (message) {
        LauncherIPC_HandleCommand(ipc, message);
    }
}

struct ClientLauncherIPC *ClientLauncherIPC_Create(void)
{
    struct ClientLauncherIPC *ipc = calloc(1, sizeof(struct ClientLauncherIPC));
    if (!ipc) return NULL;

    ipc->base.handle_message = ClientLauncherIPC_HandleMessage;
    ipc->base.handle_command = LauncherIPC_HandleCommand;

    return ipc;
}

void ClientLauncherIPC_Free(struct ClientLauncherIPC *ipc)
{
    if (!ipc) return;

    LauncherIPC_Free(&ipc->base);
    free(ipc);
}