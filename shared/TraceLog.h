/* TraceLog — C port of shared/TraceLog.pas */

#ifndef TRACELOG_H
#define TRACELOG_H

#include <stdbool.h>

#define LEVEL_OFF 0
#define LEVEL_DEBUG 1
#define LEVEL_TRACE 2

void TraceLog_Debug(const char *fmt, ...);
void TraceLog_Trace(const char *fmt, ...);

#endif /* TRACELOG_H */