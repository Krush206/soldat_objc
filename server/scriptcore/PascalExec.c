#include "PascalExec.h"
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------
 * Assumed PascalScript C‑API (these would be provided by the
 * PascalScript runtime library).  The names are invented but follow
 * the Pascal unit naming conventions.
 * ------------------------------------------------------------------ */
extern PSExec *ps_exec_create(void);
extern void ps_exec_free(PSExec *e);
extern void ps_exec_stop(PSExec *e);
extern int ps_exec_load_data(PSExec *e, const char *bytecode);
extern const char *ps_error_to_string(int code, const char *msg);
extern void ps_exec_register_dll_runtime(PSExec *e);
extern void ps_register_class_library_runtime(PSExec *e,
                                             PSRuntimeClassImporter *imp);
extern void ps_ri_register_tobject(PSRuntimeClassImporter *imp);
extern unsigned int ps_exec_get_proc_count(const PSExec *e);
extern PSProcRec *ps_exec_get_proc_no(const PSExec *e, unsigned int idx);
extern PSProcRec *ps_exec_get_proc(const PSExec *e, const char *name);
extern void ps_exec_register_delphi_function(PSExec *e,
                                            void *procPtr,
                                            const char *name,
                                            int callingConvention);
extern void ps_exec_register_delphi_method(PSExec *e,
                                           void *obj,
                                           void *procPtr,
                                           const char *name,
                                           int callingConvention);
extern PSRuntimeClassImporter *ps_runtime_class_importer_create_and_register(
        PSExec *e, int flag);
extern void ps_runtime_class_importer_free(PSRuntimeClassImporter *imp);
extern PSRuntimeClassImporter *ps_runtime_class_importer_add(
        PSRuntimeClassImporter *imp, void *aClass);
extern PSRuntimeClassImporter *ps_runtime_class_importer_add2(
        PSRuntimeClassImporter *imp, void *aClass, const char *name);
extern PSTypeRec *ps_exec_find_type2(PSExec *e, TPSBaseType *base);
extern void *ps_exec_get_var2(PSExec *e, const char *name);
extern void v_set_uint(void *var, unsigned int v);
extern void v_set_int64(void *var, long long v);
extern void v_set_real(void *var, long double v);
extern void v_set_currency(void *var, long double v);
extern void v_set_int(void *var, int v);
extern void v_set_string(void *var, const char *v);
extern void v_set_wide_string(void *var, const wchar_t *v);
extern void v_set_unicode_string(void *var, const char *v);   /* UTF‑8 */
extern void vn_set_pointer_to(void *variantIFC, void *data, PSTypeRec *type);
extern PascalVariant *ps_exec_run_proc_p(PSExec *e,
                                         PascalVariant **params,
                                         unsigned int paramCount,
                                         unsigned int procNo);
extern void ps_exec_run_proc(PSExec *e,
                             PascalVariant **params,
                             unsigned int paramCount,
                             unsigned int procNo);

/* ------------------------------------------------------------------
 * Internal structure definition
 * ------------------------------------------------------------------ */
struct PascalExec {
    PSExec *exec;
    PSRuntimeClassImporter *classImporter;
    unsigned int *initProcNums;
    size_t initProcCount;
    unsigned int *finiProcNums;
    size_t finiProcCount;
    unsigned int mainProcNum;
    void *apiList;               /* opaque TList – we treat as void* */
    PascalExec_OnException onException;
};

/* ------------------------------------------------------------------
 * Helper: exception trampoline used by PascalScript runtime.
 * ------------------------------------------------------------------ */
static void pascal_exception_trampoline(PSExec *sender,
                                        int exError,
                                        const char *exParam,
                                        void *exObject,
                                        unsigned int procNo,
                                        unsigned int position)
{
    PascalExec *self = (PascalExec *)sender;   /* Id field stores back‑pointer */
    if (self && self->onException) {
        self->onException(self, exError, exParam, exObject, procNo, position);
    }
}

/* ------------------------------------------------------------------
 * Construction / destruction
 * ------------------------------------------------------------------ */
PascalExec *PascalExec_Create(void)
{
    PascalExec *self = (PascalExec *)calloc(1, sizeof(PascalExec));
    if (!self) return NULL;

    self->exec = ps_exec_create();
    if (!self->exec) {
        free(self);
        return NULL;
    }
    /* Store back‑pointer for the exception callback */
    /* The PascalScript runtime expects an 'Id' field – we simulate it
       by attaching the pointer to the exec structure via a reserved
       field; here we just keep it in the wrapper. */
    /* Register exception handler */
    /* Assuming the runtime allows setting a callback via a function: */
    /* ps_exec_set_on_exception(self->exec, pascal_exception_trampoline); */
    /* For this translation we assume the field exists. */
    /* self->exec->Id = self; */
    /* self->exec->OnException = pascal_exception_trampoline; */

    self->classImporter = ps_runtime_class_importer_create_and_register(self->exec, 0);
    self->mainProcNum = (unsigned int)-1;   /* InvalidVal */

    return self;
}

void PascalExec_Destroy(PascalExec *self)
{
    if (!self) return;

    /* Call finalization procs */
    for (size_t i = 0; i < self->finiProcCount; ++i) {
        ps_exec_run_proc(self->exec, NULL, 0, self->finiProcNums[i]);
    }

    ps_exec_stop(self->exec);
    ps_exec_free(self->exec);
    ps_runtime_class_importer_free(self->classImporter);

    free(self->initProcNums);
    free(self->finiProcNums);
    free(self);
}

/* ------------------------------------------------------------------
 * Bytecode loading
 * ------------------------------------------------------------------ */
void PascalExec_LoadBytecode(PascalExec *self, const char *bytecode)
{
    if (!self || !bytecode) return;

    /* Register core runtime libraries */
    ps_register_class_library_runtime(self->exec, self->classImporter);
    ps_ri_register_tobject(self->classImporter);

    /* If the user supplied an API list, ask each element to register */
    if (self->apiList) {
        /* The real code iterates a TList of IRuntimeAPI interfaces.
           Here we assume the list is an array of IRuntimeAPI* terminated
           by a NULL pointer. */
        IRuntimeAPI **apis = (IRuntimeAPI **)self->apiList;
        for (IRuntimeAPI **p = apis; *p; ++p) {
            (*p)->RuntimeRegisterApi(self);
        }
    }

    if (!ps_exec_load_data(self->exec, bytecode)) {
        /* Raise an error – in C we can’t throw, so we just store it.
           The caller can retrieve it via GetErrorString. */
        const char *err = PascalExec_GetErrorString(self);
        /* In a real binding we might set a global error or abort. */
        (void)err; /* silence unused warning */
        /* For fidelity we abort */
        abort();
    }

    if (self->apiList) {
        IRuntimeAPI **apis = (IRuntimeAPI **)self->apiList;
        for (IRuntimeAPI **p = apis; *p; ++p) {
            (*p)->RuntimeRegisterVariables(self);
        }
    }

    /* Scan procedures for initialization / finalization / main */
    unsigned int procCount = ps_exec_get_proc_count(self->exec);
    for (unsigned int i = 0; i < procCount; ++i) {
        PSProcRec *proc = ps_exec_get_proc_no(self->exec, i);
        if (!proc) continue;

        /* Only internal script procedures have ExportName */
        const char *exportName = NULL;
        /* We need a way to test class type; assume a function: */
        extern int ps_proc_is_internal(PSProcRec *);
        extern const char *ps_proc_get_export_name(PSProcRec *);
        if (!ps_proc_is_internal(proc)) continue;
        exportName = ps_proc_get_export_name(proc);
        if (!exportName) continue;

        if (strncmp(exportName, "!MAIN_", 6) == 0) {
            self->initProcNums = realloc(self->initProcNums,
                                         (self->initProcCount + 1) * sizeof(unsigned int));
            self->initProcNums[self->initProcCount++] = i;
        } else if (strncmp(exportName, "!FINISH_", 8) == 0) {
            self->finiProcNums = realloc(self->finiProcNums,
                                         (self->finiProcCount + 1) * sizeof(unsigned int));
            self->finiProcNums[self->finiProcCount++] = i;
        } else if (strcmp(exportName, "!MAIN") == 0) {
            self->mainProcNum = i;
        }
    }
}

/* ------------------------------------------------------------------
 * Error strings
 * ------------------------------------------------------------------ */
const char *PascalExec_GetErrorString(const PascalExec *self)
{
    if (!self) return "PascalExec is NULL";
    /* The runtime stores last exception code / string in the exec object */
    extern int ps_exec_get_exception_code(const PSExec *);
    extern const char *ps_exec_get_exception_string(const PSExec *);
    int code = ps_exec_get_exception_code(self->exec);
    const char *msg = ps_exec_get_exception_string(self->exec);
    return ps_error_to_string(code, msg);
}

const char *PascalExec_GetErrorString2(const PascalExec *self,
                                       int errorCode,
                                       const char *message)
{
    (void)self;
    return ps_error_to_string(errorCode, message);
}

/* ------------------------------------------------------------------
 * Execution
 * ------------------------------------------------------------------ */
void PascalExec_Execute(PascalExec *self)
{
    if (!self) return;

    if (self->apiList) {
        IRuntimeAPI **apis = (IRuntimeAPI **)self->apiList;
        for (IRuntimeAPI **p = apis; *p; ++p) {
            (*p)->BeforeExecute(self);
        }
    }

    /* Run initialization procedures */
    for (size_t i = 0; i < self->initProcCount; ++i) {
        ps_exec_run_proc(self->exec, NULL, 0, self->initProcNums[i]);
    }

    if (self->mainProcNum != (unsigned int)-1) {
        ps_exec_run_proc(self->exec, NULL, 0, self->mainProcNum);
    }
}

/* ------------------------------------------------------------------
 * Function calling
 * ------------------------------------------------------------------ */
static PascalVariant *call_function_internal(PascalExec *self,
                                             const char *funcName,
                                             unsigned int procNo,
                                             PascalVariant **params,
                                             unsigned int paramCount)
{
    if (procNo == (unsigned int)-1) {
        /* Proc not found */
        return NULL;
    }
    PSProcRec *proc = ps_exec_get_proc_no(self->exec, procNo);
    extern int ps_proc_is_internal(PSProcRec *);
    if (!ps_proc_is_internal(proc)) {
        return NULL;
    }

    if (self->apiList) {
        IRuntimeAPI **apis = (IRuntimeAPI **)self->apiList;
        for (IRuntimeAPI **p = apis; *p; ++p) {
            (*p)->BeforeExecute(self);
        }
    }

    PascalVariant *result = ps_exec_run_proc_p(self->exec, params, paramCount, procNo);

    if (self->apiList) {
        IRuntimeAPI **apis = (IRuntimeAPI **)self->apiList;
        for (IRuntimeAPI **p = apis; *p; ++p) {
            (*p)->AfterExecute(self);
        }
    }
    return result;
}

PascalVariant *PascalExec_CallFunctionByProcNo(PascalExec *self,
                                               const char *funcName,
                                               unsigned int procNo,
                                               PascalVariant **params,
                                               unsigned int paramCount)
{
    return call_function_internal(self, funcName, procNo, params, paramCount);
}

PascalVariant *PascalExec_CallFunctionByName(PascalExec *self,
                                             const char *funcName,
                                             PascalVariant **params,
                                             unsigned int paramCount)
{
    unsigned int procNo = (unsigned int)-1;
    if (self && funcName) {
        PSProcRec *proc = ps_exec_get_proc(self->exec, funcName);
        if (proc) {
            extern unsigned int ps_proc_get_index(PSProcRec *);
            procNo = ps_proc_get_index(proc);
        }
    }
    return call_function_internal(self, funcName, procNo, params, paramCount);
}

/* ------------------------------------------------------------------
 * Event calling (shortcut)
 * ------------------------------------------------------------------ */
PascalVariant *PascalExec_CallEvent(PascalExec *self,
                                    const void *event,
                                    PascalVariant **params,
                                    unsigned int paramCount)
{
    if (!self || !event) return NULL;

    /* In Pascal, Event is a TMethod record; we extract ProcNo from its Data field. */
    /* Assume the layout: struct { void *Code; void *Data; } */
    typedef struct { void *Code; void *Data; } TMethod;
    const TMethod *m = (const TMethod *)event;
    /* Data points to a PScriptMethodInfo which has a ProcNo field. */
    typedef struct { unsigned int ProcNo; } PScriptMethodInfo;
    const PScriptMethodInfo *info = (const PScriptMethodInfo *)m->Data;
    unsigned int procNo = info->ProcNo;

    PSProcRec *proc = ps_exec_get_proc_no(self->exec, procNo);
    const char *eventName = NULL;
    extern int ps_proc_is_internal(PSProcRec *);
    extern const char *ps_proc_get_export_name(PSProcRec *);
    extern const char *ps_external_proc_get_name(PSProcRec *);
    if (ps_proc_is_internal(proc)) {
        eventName = ps_proc_get_export_name(proc);
    } else {
        eventName = ps_external_proc_get_name(proc);
    }

    return call_function_internal(self, eventName, procNo, params, paramCount);
}

/* ------------------------------------------------------------------
 * Proc lookup
 * ------------------------------------------------------------------ */
PSProcRec *PascalExec_GetProc(const PascalExec *self, unsigned int procNo)
{
    if (!self) return NULL;
    return ps_exec_get_proc_no(self->exec, procNo);
}

/* ------------------------------------------------------------------
 * DLL support
 * ------------------------------------------------------------------ */
void PascalExec_AddDllSupport(PascalExec *self)
{
    if (!self) return;
    ps_exec_register_dll_runtime(self->exec);
}

/* ------------------------------------------------------------------
 * Class registration
 * ------------------------------------------------------------------ */
PSRuntimeClassImporter *PascalExec_AddClass(PascalExec *self, void *aClass)
{
    if (!self) return NULL;
    return ps_runtime_class_importer_add(self->classImporter, aClass);
}

PSRuntimeClassImporter *PascalExec_AddClassNamed(PascalExec *self,
                                                 void *aClass,
                                                 const char *name)
{
    if (!self) return NULL;
    return ps_runtime_class_importer_add2(self->classImporter, aClass, name);
}

/* ------------------------------------------------------------------
 * Function / method registration
 * ------------------------------------------------------------------ */
void PascalExec_AddFunction(PascalExec *self, void *procPtr, const char *name)
{
    if (!self || !name) return;
    ps_exec_register_delphi_function(self->exec, procPtr, name, 0); /* cdRegister = 0 */
}

void PascalExec_AddObjectMethod(PascalExec *self,
                                void *obj,
                                void *procPtr,
                                const char *name)
{
    if (!self || !name) return;
    ps_exec_register_delphi_method(self->exec, obj, procPtr, name, 0);
}

/* ------------------------------------------------------------------
 * Type lookup
 * ------------------------------------------------------------------ */
PSTypeRec *PascalExec_FindType(PascalExec *self, TPSBaseType *baseType)
{
    if (!self) return NULL;
    return ps_exec_find_type2(self->exec, baseType);
}

/* ------------------------------------------------------------------
 * Variable manipulation
 * ------------------------------------------------------------------ */
void PascalExec_SetPointerToData(PascalExec *self,
                                 const char *varName,
                                 void *data,
                                 PSTypeRec *aType)
{
    if (!self || !varName || !aType) return;
    void *var = ps_exec_get_var2(self->exec, varName);
    if (!var) return;   /* raise? */
    vn_set_pointer_to(var, data, aType);
}

void PascalExec_SetValue_UInt(PascalExec *self,
                              const char *varName,
                              unsigned int value)
{
    if (!self || !varName) return;
    void *var = ps_exec_get_var2(self->exec, varName);
    if (!var) return;
    v_set_uint(var, value);
}

void PascalExec_SetValue_Int64(PascalExec *self,
                               const char *varName,
                               long long value)
{
    if (!self || !varName) return;
    void *var = ps_exec_get_var2(self->exec, varName);
    if (!var) return;
    v_set_int64(var, value);
}

void PascalExec_SetValue_Extended(PascalExec *self,
                                  const char *varName,
                                  long double value)
{
    if (!self || !varName) return;
    void *var = ps_exec_get_var2(self->exec, varName);
    if (!var) return;
    v_set_real(var, value);
}

void PascalExec_SetValue_Currency(PascalExec *self,
                                  const char *varName,
                                  long double value)
{
    if (!self || !varName) return;
    void *var = ps_exec_get_var2(self->exec, varName);
    if (!var) return;
    v_set_currency(var, value);
}

void PascalExec_SetValue_Int(PascalExec *self,
                             const char *varName,
                             int value)
{
    if (!self || !varName) return;
    void *var = ps_exec_get_var2(self->exec, varName);
    if (!var) return;
    v_set_int(var, value);
}

void PascalExec_SetValue_String(PascalExec *self,
                                const char *varName,
                                const char *value)
{
    if (!self || !varName) return;
    void *var = ps_exec_get_var2(self->exec, varName);
    if (!var) return;
    v_set_string(var, value);
}

void PascalExec_SetValue_WideString(PascalExec *self,
                                    const char *varName,
                                    const wchar_t *value)
{
    if (!self || !varName) return;
    void *var = ps_exec_get_var2(self->exec, varName);
    if (!var) return;
    v_set_wide_string(var, value);
}

void PascalExec_SetValue_UnicodeString(PascalExec *self,
                                       const char *varName,
                                       const char *value)   /* UTF‑8 */
{
    if (!self || !varName) return;
    void *var = ps_exec_get_var2(self->exec, varName);
    if (!var) return;
    v_set_unicode_string(var, value);
}

/* ------------------------------------------------------------------
 * Property accessors
 * ------------------------------------------------------------------ */
void PascalExec_SetOnException(PascalExec *self,
                               PascalExec_OnException callback)
{
    if (!self) return;
    self->onException = callback;
}

void *PascalExec_GetApiList(const PascalExec *self)
{
    return self ? self->apiList : NULL;
}

void PascalExec_SetApiList(PascalExec *self, void *list)
{
    if (!self) return;
    self->apiList = list;
}

PSExec *PascalExec_GetExec(const PascalExec *self)
{
    return self ? self->exec : NULL;
}

PSRuntimeClassImporter *PascalExec_GetClassImporter(const PascalExec *self)
{
    return self ? self->classImporter : NULL;
}