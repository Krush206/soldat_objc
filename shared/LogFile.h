/* LogFile — C port of shared/LogFile.pas */

#ifndef LOGFILE_H
#define LOGFILE_H

#include "Classes.h"
#include "SyncObjs.h"

extern StringList *GameLog;
extern char *ConsoleLogFileName;
extern CriticalSection *LogLock;

void LogFile_NewLogFile(StringList **f, const char *name);
void LogFile_WriteLogFile(StringList *f, const char *name);
void LogFile_AddLineToLogFile(StringList **f, const char *s, const char *name, bool with_date);
void LogFile_NewLogFiles(void);
void LogFile_InitializeUnit(void);

#endif /* LOGFILE_H */