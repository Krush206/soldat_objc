#include "PascalDebugger.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Forward declarations of the low‑level PascalScript debugger API.
   These are assumed to be provided by the uPSRuntime / uPSDebugger C bindings. */
typedef struct TPSDebugExec TPSDebugExec;

extern TPSDebugExec *TPSDebugExec_Create(void);
extern void TPSDebugExec_Destroy(TPSDebugExec *exec);
extern void TPSDebugExec_LoadDebugData(TPSDebugExec *exec, const char *data);
extern void TPSDebugExec_SetDebugEnabled(TPSDebugExec *exec, int enabled);
extern TPSError TPSDebugExec_GetExceptionCode(const TPSDebugExec *exec);
extern const char *TPSDebugExec_GetExceptionString(const TPSDebugExec *exec);
extern unsigned int TPSDebugExec_GetExceptionProcNo(const TPSDebugExec *exec);
extern unsigned int TPSDebugExec_GetExceptionPos(const TPSDebugExec *exec);
extern int TPSDebugExec_TranslatePositionEx(const TPSDebugExec *exec,
                                           unsigned int procNo,
                                           unsigned int position,
                                           unsigned int *pos,
                                           unsigned int *row,
                                           unsigned int *col,
                                           char **unitName);
extern const char *TIFErrorToString(TPSError code, const char *msg);

/* Internal representation of the debugger object */
struct PascalDebugger {
    TPSDebugExec *exec;
};

/* --------------------------------------------------------------- */
/* Construction / destruction                                        */
/* --------------------------------------------------------------- */
PascalDebugger *PascalDebugger_Create(void)
{
    PascalDebugger *dbg = (PascalDebugger *)malloc(sizeof(PascalDebugger));
    if (!dbg)
        return NULL;

    dbg->exec = TPSDebugExec_Create();
    if (!dbg->exec) {
        free(dbg);
        return NULL;
    }

    /* Enable debugging as the Pascal constructor does */
    TPSDebugExec_SetDebugEnabled(dbg->exec, 1);
    return dbg;
}

void PascalDebugger_Destroy(PascalDebugger *dbg)
{
    if (!dbg)
        return;
    if (dbg->exec)
        TPSDebugExec_Destroy(dbg->exec);
    free(dbg);
}

/* --------------------------------------------------------------- */
/* Property: DebugSymbols (write only)                               */
/* --------------------------------------------------------------- */
void PascalDebugger_SetDebugSymbols(PascalDebugger *dbg, const char *debugSymbols)
{
    if (!dbg || !dbg->exec || !debugSymbols)
        return;
    TPSDebugExec_LoadDebugData(dbg->exec, debugSymbols);
}

/* --------------------------------------------------------------- */
/* Property: LastError (read only)                                   */
/* --------------------------------------------------------------- */
TPSError PascalDebugger_GetLastError(const PascalDebugger *dbg)
{
    if (!dbg || !dbg->exec)
        return 0;               /* assume 0 == no error */
    return TPSDebugExec_GetExceptionCode(dbg->exec);
}

/* --------------------------------------------------------------- */
/* Helper: format error string using the low‑level routine           */
/* --------------------------------------------------------------- */
static const char *FormatErrorString(const PascalDebugger *dbg,
                                    TPSError exCode,
                                    const char *exString,
                                    unsigned int procNo,
                                    unsigned int position)
{
    static char buffer[1024];
    unsigned int dummyPos = 0, row = 0, col = 0;
    char *unitName = NULL;

    if (TPSDebugExec_TranslatePositionEx(dbg->exec, procNo, position,
                                         &dummyPos, &row, &col, &unitName)) {
        snprintf(buffer, sizeof(buffer),
                 "In unit %s(%u:%u): %s",
                 unitName ? unitName : "",
                 row, col,
                 TIFErrorToString(exCode, exString));
        if (unitName)
            free(unitName);    /* the low‑level API returns an allocated string */
    } else {
        snprintf(buffer, sizeof(buffer), "%s",
                 TIFErrorToString(exCode, exString));
    }
    return buffer;
}

/* --------------------------------------------------------------- */
/* GetErrorString – overload without parameters (last error)        */
/* --------------------------------------------------------------- */
const char *PascalDebugger_GetErrorString(const PascalDebugger *dbg)
{
    if (!dbg || !dbg->exec)
        return "Debugger not initialized";

    TPSError code = TPSDebugExec_GetExceptionCode(dbg->exec);
    const char *msg = TPSDebugExec_GetExceptionString(dbg->exec);
    unsigned int procNo = TPSDebugExec_GetExceptionProcNo(dbg->exec);
    unsigned int pos = TPSDebugExec_GetExceptionPos(dbg->exec);

    return FormatErrorString(dbg, code, msg, procNo, pos);
}

/* --------------------------------------------------------------- */
/* GetErrorStringEx – explicit error information                    */
/* --------------------------------------------------------------- */
const char *PascalDebugger_GetErrorStringEx(const PascalDebugger *dbg,
                                            TPSError exCode,
                                            const char *exString,
                                            unsigned int procNo,
                                            unsigned int position)
{
    if (!dbg || !dbg->exec)
        return "Debugger not initialized";

    return FormatErrorString(dbg, exCode, exString ? exString : "", procNo, position);
}

/* --------------------------------------------------------------- */
/* GetPosition – last exception location                            */
/* --------------------------------------------------------------- */
void PascalDebugger_GetPosition(const PascalDebugger *dbg,
                                char **unitName,
                                unsigned int *col,
                                unsigned int *row)
{
    if (!dbg || !dbg->exec) {
        if (unitName) *unitName = NULL;
        if (col) *col = 0;
        if (row) *row = 0;
        return;
    }

    unsigned int dummyPos = 0;
    unsigned int procNo = TPSDebugExec_GetExceptionProcNo(dbg->exec);
    unsigned int pos = TPSDebugExec_GetExceptionPos(dbg->exec);

    char *uName = NULL;
    TPSDebugExec_TranslatePositionEx(dbg->exec, procNo, pos,
                                     &dummyPos, row, col, &uName);
    if (unitName)
        *unitName = uName;   /* caller owns the string */
    else if (uName)
        free(uName);
}

/* --------------------------------------------------------------- */
/* GetPositionEx – arbitrary ProcNo / Position                       */
/* --------------------------------------------------------------- */
void PascalDebugger_GetPositionEx(const PascalDebugger *dbg,
                                  char **unitName,
                                  unsigned int *col,
                                  unsigned int *row,
                                  unsigned int procNo,
                                  unsigned int position)
{
    if (!dbg || !dbg->exec) {
        if (unitName) *unitName = NULL;
        if (col) *col = 0;
        if (row) *row = 0;
        return;
    }

    unsigned int dummyPos = 0;
    char *uName = NULL;
    TPSDebugExec_TranslatePositionEx(dbg->exec, procNo, position,
                                     &dummyPos, row, col, &uName);
    if (unitName)
        *unitName = uName;
    else if (uName)
        free(uName);
}