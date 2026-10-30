#ifndef PASCALEXEC_H
#define PASCALEXEC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque handle for the Pascal execution engine */
typedef struct PascalExec PascalExec;

/* Callback type for exception handling */
typedef void (*PascalExec_OnException)(PascalExec *exec,
                                       int exError,
                                       const char *exParam,
                                       void *exObject,
                                       unsigned int procNo,
                                       unsigned int position);

/* Variant type used for parameters and return values.
   In a real binding this would be a rich union; here we keep it opaque. */
typedef struct PascalVariant PascalVariant;

/* Forward declarations of PascalScript runtime types used in the API */
typedef struct PSExec PSExec;
typedef struct PSRuntimeClassImporter PSRuntimeClassImporter;
typedef struct PSProcRec PSProcRec;
typedef struct PSTypeRec PSTypeRec;
typedef struct TPSBaseType TPSBaseType;
typedef struct IRuntimeAPI IRuntimeAPI;

/* Constants mirrored from uPSUtils */
enum PascalBaseType {
    btReturnAddress = 0,
    btU8 = 1,
    btS8 = 2,
    btU16 = 3,
    btS16 = 4,
    btU32 = 5,
    btS32 = 6,
    btSingle = 7,
    btDouble = 8,
    btExtended = 9,
    btString = 10,
    btRecord = 11,
    btArray = 12,
    btPointer = 13,
    btPChar = 14,
    btResourcePointer = 15,
    btVariant = 16,
    btS64 = 17,
    btChar = 18,
    btWideString = 19,
    btWideChar = 20,
    btProcPtr = 21,
    btStaticArray = 22,
    btSet = 23,
    btCurrency = 24,
    btClass = 25,
    btInterface = 26,
    btNotificationVariant = 27,
    btUnicodeString = 28,
    btType = 29,
    btEnum = 30,
    btExtClass = 31
};

/* Construction / destruction */
PascalExec *PascalExec_Create(void);
void PascalExec_Destroy(PascalExec *exec);

/* Bytecode loading (property setter) */
void PascalExec_LoadBytecode(PascalExec *exec, const char *bytecode);

/* Error reporting (property getter) */
const char *PascalExec_GetErrorString(const PascalExec *exec);
const char *PascalExec_GetErrorString2(const PascalExec *exec,
                                       int errorCode,
                                       const char *message);

/* Execution control */
void PascalExec_Execute(PascalExec *exec);

/* Function calling */
PascalVariant *PascalExec_CallFunctionByName(PascalExec *exec,
                                             const char *funcName,
                                             PascalVariant **params,
                                             unsigned int paramCount);
PascalVariant *PascalExec_CallFunctionByProcNo(PascalExec *exec,
                                               const char *funcName,
                                               unsigned int procNo,
                                               PascalVariant **params,
                                               unsigned int paramCount);

/* Event calling (special shortcut) */
PascalVariant *PascalExec_CallEvent(PascalExec *exec,
                                    const void *event,
                                    PascalVariant **params,
                                    unsigned int paramCount);

/* Proc lookup */
PSProcRec *PascalExec_GetProc(const PascalExec *exec, unsigned int procNo);

/* DLL support */
void PascalExec_AddDllSupport(PascalExec *exec);

/* Class registration */
PSRuntimeClassImporter *PascalExec_AddClass(PascalExec *exec, void *aClass);
PSRuntimeClassImporter *PascalExec_AddClassNamed(PascalExec *exec,
                                                void *aClass,
                                                const char *name);

/* Function / method registration */
void PascalExec_AddFunction(PascalExec *exec, void *procPtr, const char *name);
void PascalExec_AddObjectMethod(PascalExec *exec,
                                void *obj,
                                void *procPtr,
                                const char *name);

/* Type lookup */
PSTypeRec *PascalExec_FindType(PascalExec *exec, TPSBaseType *baseType);

/* Variable manipulation */
void PascalExec_SetPointerToData(PascalExec *exec,
                                 const char *varName,
                                 void *data,
                                 PSTypeRec *aType);
void PascalExec_SetValue_UInt(PascalExec *exec,
                              const char *varName,
                              unsigned int value);
void PascalExec_SetValue_Int64(PascalExec *exec,
                               const char *varName,
                               long long value);
void PascalExec_SetValue_Extended(PascalExec *exec,
                                  const char *varName,
                                  long double value);
void PascalExec_SetValue_Currency(PascalExec *exec,
                                  const char *varName,
                                  long double value);   /* Currency maps to 64‑bit fixed */
void PascalExec_SetValue_Int(PascalExec *exec,
                             const char *varName,
                             int value);
void PascalExec_SetValue_String(PascalExec *exec,
                                const char *varName,
                                const char *value);
void PascalExec_SetValue_WideString(PascalExec *exec,
                                    const char *varName,
                                    const wchar_t *value);
void PascalExec_SetValue_UnicodeString(PascalExec *exec,
                                       const char *varName,
                                       const char *value);   /* UTF‑8 */

/* Property accessors */
void PascalExec_SetOnException(PascalExec *exec,
                               PascalExec_OnException callback);
void *PascalExec_GetApiList(const PascalExec *exec);
void PascalExec_SetApiList(PascalExec *exec, void *list);
PSExec *PascalExec_GetExec(const PascalExec *exec);
PSRuntimeClassImporter *PascalExec_GetClassImporter(const PascalExec *exec);

#ifdef __cplusplus
}
#endif

#endif /* PASCALEXEC_H */