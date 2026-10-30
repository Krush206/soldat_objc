/* LauncherIPC — C port of shared/LauncherIPC.pas */

#include "LauncherIPC.h"
#include "TraceLog.h"
#include "Command.h"

#include <stdlib.h>

/* Static variable to hold the current IPC for callback */
static struct LauncherIPC *current_ipc = NULL;

/* Wrapper function for LauncherConnection callback */
static void LauncherIPC_MessageWrapper(const char *message)
{
    if (current_ipc && current_ipc->handle_message) {
        current_ipc->handle_message(current_ipc, message);
    }
}

struct LauncherIPC *LauncherIPC_Create(LauncherIPC_HandleMessageFunc handle_message)
{
    struct LauncherIPC *ipc = calloc(1, sizeof(struct LauncherIPC));
    if (!ipc) return NULL;

    ipc->handle_message = handle_message;
    ipc->handle_command = LauncherIPC_HandleCommand;
    ipc->thread_alive = false;

    return ipc;
}

void LauncherIPC_Free(struct LauncherIPC *ipc)
{
    if (!ipc) return;

    if (ipc->thread_alive) {
        TraceLog_Debug("[LauncherIPC] Killing thread...");
        LauncherConnection_Free(ipc->connection);
        ipc->connection = NULL;
        ipc->thread_alive = false;
    }

    free(ipc);
}

void LauncherIPC_Connect(struct LauncherIPC *ipc, int port)
{
    if (!ipc) return;

    if (ipc->thread_alive) {
        LauncherConnection_Free(ipc->connection);
        ipc->connection = NULL;
        ipc->thread_alive = false;
    }

    TraceLog_Debug("[LauncherIPC] Starting launcher connection thread");
    ipc->connection = LauncherConnection_Create(port);
    if (ipc->connection) {
        current_ipc = ipc;
        LauncherConnection_SetMessageCallback(ipc->connection, LauncherIPC_MessageWrapper);
        ipc->thread_alive = true;
    }
}

void LauncherIPC_SendMessage(struct LauncherIPC *ipc, const char *message)
{
    if (!ipc || !ipc->thread_alive || !ipc->connection) return;
    LauncherConnection_SendMessage(ipc->connection, message);
}

bool LauncherIPC_ThreadAlive(struct LauncherIPC *ipc)
{
    return ipc && ipc->thread_alive;
}

void LauncherIPC_HandleCommand(struct LauncherIPC *ipc, const char *message)
{
    (void)ipc;
    if (message) {
        Command_ParseInput(message, 255);
    }
}