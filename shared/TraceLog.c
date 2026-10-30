/* TraceLog — C port of shared/TraceLog.pas */

#include "TraceLog.h"
#include <stdio.h>
#include <stdarg.h>

static int current_level = LEVEL_DEBUG;

void TraceLog_Debug(const char *fmt, ...)
{
    if (current_level >= LEVEL_DEBUG) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "[DEBUG] ");
        vfprintf(stderr, fmt, args);
        fprintf(stderr, "\n");
        va_end(args);
    }
}

void TraceLog_Trace(const char *fmt, ...)
{
    if (current_level >= LEVEL_TRACE) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "[TRACE] ");
        vfprintf(stderr, fmt, args);
        fprintf(stderr, "\n");
        va_end(args);
    }
}