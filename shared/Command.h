/* Command — C port of shared/Command.pas */

#ifndef COMMAND_H
#define COMMAND_H

#include "SysUtils.h"
#include "Classes.h"

#define MAX_COMMANDS 1024

/* TCommandFlag set */
#define CMD_INIT (1 << 0)
#define CMD_ALIAS (1 << 1)
#define CMD_SCRIPT (1 << 2)
#define CMD_DEFERRED (1 << 3)
#define CMD_ADMINONLY (1 << 4)
#define CMD_PLAYERONLY (1 << 5)

typedef int TCommandFlags;

typedef void (*TCommandFunction)(char **args, int argsCount,
    unsigned char sender);

typedef struct TCommand {
    char *Name;
    TCommandFunction FunctionPtr;
    char *Description;
    TCommandFlags Flags;
} TCommand;
typedef TCommand *PCommand;

typedef struct TCommandTargets {
    unsigned char *targets;
    int count;
} TCommandTargets;

extern TCommand *Commands[MAX_COMMANDS];
extern char *CommandNames[MAX_COMMANDS];
extern int CommandsCount;
extern StringList *DeferredCommands;
extern bool DeferredInitialized;

void Command_Init(void);
bool Command_ParseInput(const char *input, unsigned char sender);
bool Command_ParseInput2(const char *input);
bool Command_LoadConfig(const char *config_name);
PCommand Command_Add(const char *command_name_var, TCommandFunction command_ptr,
    const char *description, TCommandFlags flags);
PCommand Command_Find(const char *name);
void Command_ExecuteAlias(char **args, int args_count, unsigned char sender);
void Command_ParseCommandLine(void);
void Command_RunDeferredCommands(void);
TCommandTargets Command_Target(const char *target, unsigned char sender);

#endif /* COMMAND_H */