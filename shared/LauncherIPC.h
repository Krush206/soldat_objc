/* LauncherIPC — C port of shared/LauncherIPC.pas */

#ifndef LAUNCHERIPC_H
#define LAUNCHERIPC_H

#include <stdbool.h>
#include "LauncherConnection.h"

struct LauncherIPC;

/* Function pointer types for virtual methods */
typedef void (*LauncherIPC_HandleMessageFunc)(struct LauncherIPC *ipc, const char *message);
typedef void (*LauncherIPC_HandleCommandFunc)(struct LauncherIPC *ipc, const char *message);

/* Opaque structure */
struct LauncherIPC {
    struct LauncherConnection *connection;
    bool thread_alive;
    LauncherIPC_HandleMessageFunc handle_message;
    LauncherIPC_HandleCommandFunc handle_command;
};

/* Create a new launcher IPC */
struct LauncherIPC *LauncherIPC_Create(LauncherIPC_HandleMessageFunc handle_message);

/* Destroy launcher IPC */
void LauncherIPC_Free(struct LauncherIPC *ipc);

/* Connect to launcher on specified port */
void LauncherIPC_Connect(struct LauncherIPC *ipc, int port);

/* Send message to launcher */
void LauncherIPC_SendMessage(struct LauncherIPC *ipc, const char *message);

/* Check if thread is alive */
bool LauncherIPC_ThreadAlive(struct LauncherIPC *ipc);

/* Default command handler */
void LauncherIPC_HandleCommand(struct LauncherIPC *ipc, const char *message);

#endif /* LAUNCHERIPC_H */