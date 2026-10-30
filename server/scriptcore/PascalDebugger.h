#ifndef PASCAL_DEBUGGER_H
#define PASCAL_DEBUGGER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declaration of the debugger object */
typedef struct PascalDebugger PascalDebugger;

/* Error code type (mirrors uPSRuntime.TPSError) */
typedef int TPSError;

/* Create a new debugger instance.
   Returns NULL on allocation failure. */
PascalDebugger *PascalDebugger_Create(void);

/* Destroy a debugger instance and free associated resources. */
void PascalDebugger_Destroy(PascalDebugger *dbg);

/* Load debug symbols (property DebugSymbols write). */
void PascalDebugger_SetDebugSymbols(PascalDebugger *dbg, const char *debugSymbols);

/* Retrieve the last error code (property LastError read). */
TPSError PascalDebugger_GetLastError(const PascalDebugger *dbg);

/* Get a human‑readable error string for the last error. */
const char *PascalDebugger_GetErrorString(const PascalDebugger *dbg);

/* Get a human‑readable error string for an explicit error code.
   The returned pointer points to an internal static buffer; it must not be freed. */
const char *PascalDebugger_GetErrorStringEx(const PascalDebugger *dbg,
                                            TPSError exCode,
                                            const char *exString,
                                            unsigned int procNo,
                                            unsigned int position);

/* Fill out UnitName, Col and Row with the source location of the last exception. */
void PascalDebugger_GetPosition(const PascalDebugger *dbg,
                                char **unitName,
                                unsigned int *col,
                                unsigned int *row);

/* Fill out UnitName, Col and Row for an arbitrary ProcNo/Position pair. */
void PascalDebugger_GetPositionEx(const PascalDebugger *dbg,
                                  char **unitName,
                                  unsigned int *col,
                                  unsigned int *row,
                                  unsigned int procNo,
                                  unsigned int position);

#ifdef __cplusplus
}
#endif

#endif /* PASCAL_DEBUGGER_H */