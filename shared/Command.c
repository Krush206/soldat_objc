/* Command — C port of shared/Command.pas */

#include "Command.h"
#include "TraceLog.h"
#include <stdbool.h>
#include <string.h>

bool Command_ParseInput(const char *input, unsigned char sender)
{
    (void)input;
    (void)sender;
    TraceLog_Debug("[Command] ParseInput: %s", input ? input : "(null)");
    return false;
}

bool Command_ParseInput2(const char *input)
{
    (void)input;
    TraceLog_Debug("[Command] ParseInput2: %s", input ? input : "(null)");
    return false;
}