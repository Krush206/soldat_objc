#ifndef PASCALCORE_H
#define PASCALCORE_H

#include <stddef.h>

/* Forward declarations */
typedef struct TPSScript TPSScript;
typedef struct TPascalCore TPascalCore;
typedef struct TThreadScript TThreadScript;

/* TPascalCore */
TPascalCore* TPascalCore_Create(const char* Dir);
void TPascalCore_Destroy(TPascalCore* self);

TPSScript* TPascalCore_GetPascalScript(const TPascalCore* self);
void TPascalCore_SetPascalScript(TPascalCore* self, TPSScript* script);

/* Thread function – Parameters is an array of void* of length paramCount */
void TPascalCore_ThreadFunc(TPascalCore* self, void** Parameters, size_t paramCount, const char* FuncName);

/* TThreadScript */
TThreadScript* TThreadScript_Create(TPascalCore* Script, void** Parameters, size_t paramCount, const char* FuncName);
void TThreadScript_Execute(TThreadScript* self);
void TThreadScript_Destroy(TThreadScript* self);

#endif /* PASCALCORE_H */