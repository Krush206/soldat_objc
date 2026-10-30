/* AutoUpdater — Objective-C port of shared/AutoUpdater.pas
 *
 * Launches the external updater binary ("solupd", expected next to the
 * game executable) with "--waitpid <pid>" so it can replace the game
 * files after this process exits.
 *
 * NOTE(port): the Pascal unit is only compiled into the program when
 * AUTOUPDATER is defined (client/soldat.lpr calls StartAutoUpdater at the
 * top of main); the ported main() may skip calling start_auto_updater.
 *
 * The FCL TProcess class (unit Process) has no separate port; the members
 * used by this unit are modelled as a plain C struct below and Execute is
 * implemented with fork/exec in AutoUpdater.m. */

#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

#import <objc/Object.h>
#import "Classes.h"

/* TProcessOptions bitmask (FPC Process unit subset used by the game). */
#define poRunSuspended 0x01
#define poWaitOnExit   0x02
#define poUsePipes     0x04
#define poStdOutToPort 0x08
#define poNewConsole   0x10
#define poNoConsole    0x20
#define poDefaultText  0x40

/* Minimal stand-in for FCL TProcess: only the members AutoUpdater uses. */
typedef struct {
	char *Executable;
	char *CurrentDirectory;
	StringList *Parameters;
	StringList *Environment;
	unsigned int Options;	/* TProcessOptions bitmask */
	int ProcessID;
	/* Parent-side pipe fds with poUsePipes (FCL Input/Output/Stderr pipe
	 * streams); -1 when unused. */
	int Input;
	int Output;
	int Stderr;
} TProcess;

extern TProcess *UpdaterProcess;

@interface AutoUpdater: Object
/* Pascal: procedure StartAutoUpdater; */
+ (void) start_auto_updater;
/* Wiring helper for the FPC RTL's ParamStr(0) (the executable path as
 * invoked): main() should call this with argv[0] before
 * start_auto_updater. Without it the port falls back to /proc/self/exe. */
+ (void) set_param_str0: (const char *) argv0;
@end

#endif /* AUTOUPDATER_H */
