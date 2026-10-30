#include "PascalCompiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/stat.h>

/* ------------------------------------------------------------------ */
/* StringList implementation                                           */
/* ------------------------------------------------------------------ */
static void StringList_Init(StringList *list) {
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

static void StringList_Free(StringList *list) {
    for (size_t i = 0; i < list->count; ++i) {
        free(list->items[i]);
    }
    free(list->items);
    list->items = NULL;
    list->count = list->capacity = 0;
}

static void StringList_Add(StringList *list, const char *str) {
    if (list->count == list->capacity) {
        size_t newCap = list->capacity ? list->capacity * 2 : 4;
        char **newItems = realloc(list->items, newCap * sizeof(char *));
        if (!newItems) return; /* out of memory */
        list->items = newItems;
        list->capacity = newCap;
    }
    list->items[list->count] = strdup(str);
    list->count++;
}

static void StringList_AddStrings(StringList *dest, const StringList *src) {
    for (size_t i = 0; i < src->count; ++i)
        StringList_Add(dest, src->items[i]);
}

/* ------------------------------------------------------------------ */
/* Helper utilities                                                    */
/* ------------------------------------------------------------------ */
static char *StrDup(const char *s) {
    return s ? strdup(s) : NULL;
}

static char *ExpandFileName(const char *path) {
    /* Very small implementation – just returns absolute path using realpath */
    char *resolved = realpath(path, NULL);
    return resolved ? resolved : StrDup(path);
}

static bool FileExists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

static char *IncludeTrailingPathDelimiter(const char *path) {
    size_t len = strlen(path);
    if (len > 0 && path[len - 1] == '/')
        return StrDup(path);
    char *withSlash = malloc(len + 2);
    strcpy(withSlash, path);
    withSlash[len] = '/';
    withSlash[len + 1] = '\0';
    return withSlash;
}

static char *FastLowerCase(const char *s) {
    size_t len = strlen(s);
    char *low = malloc(len + 1);
    for (size_t i = 0; i < len; ++i)
        low[i] = (char)tolower((unsigned char)s[i]);
    low[len] = '\0';
    return low;
}

/* ------------------------------------------------------------------ */
/* Forward declarations of PascalScript library functions (assumed)   */
/* ------------------------------------------------------------------ */
extern TPSPreProcessor *PSPreProcessor_Create(void);
extern void PSPreProcessor_Free(TPSPreProcessor *pp);
extern void PSPreProcessor_SetID(TPSPreProcessor *pp, void *id);
extern void PSPreProcessor_SetOnNeedFile(TPSPreProcessor *pp,
                                         bool (*callback)(TPSPreProcessor *,
                                                          const char *,
                                                          char **,
                                                          char **));
extern void PSPreProcessor_SetDefines(TPSPreProcessor *pp, StringList *defines);
extern void PSPreProcessor_SetMainFileName(TPSPreProcessor *pp, const char *name);
extern void PSPreProcessor_SetMainFile(TPSPreProcessor *pp, const char *source);
extern void PSPreProcessor_PreProcess(TPSPreProcessor *pp,
                                      const char *mainFileName,
                                      char **output);
extern void PSPreProcessor_AdjustMessages(TPSPreProcessor *pp,
                                          TPSPascalCompiler *compiler);
extern TPSLineInfoResults PSPreProcessor_CurrentLineInfo_GetLineInfo(
        TPSPreProcessor *pp, const char *name, uint32_t pos,
        TPSLineInfoResults *res); /* simplified */

extern TPSPascalCompiler *TPSPascalCompiler_Create(void);
extern void TPSPascalCompiler_Free(TPSPascalCompiler *c);
extern void TPSPascalCompiler_SetID(TPSPascalCompiler *c, void *id);
extern void TPSPascalCompiler_SetOnUses(TPSPascalCompiler *c,
                                        bool (*callback)(TPSPascalCompiler *,
                                                         const char *));
extern void TPSPascalCompiler_SetOnTranslateLineInfo(TPSPascalCompiler *c,
        void (*callback)(TPSPascalCompiler *, uint32_t *, uint32_t *,
                         uint32_t *, char **));
extern bool TPSPascalCompiler_Compile(TPSPascalCompiler *c, const char *source);
extern void TPSPascalCompiler_GetOutput(TPSPascalCompiler *c, char **out);
extern void TPSPascalCompiler_GetDebugOutput(TPSPascalCompiler *c, char **out);
extern int TPSPascalCompiler_GetMsgCount(TPSPascalCompiler *c);
extern const char *TPSPascalCompiler_GetMsg(TPSPascalCompiler *c, int index);

/* Compiler registration helpers (PascalScript) */
extern void RegisterDll_Compiletime(TPSPascalCompiler *c);
extern void SIRegisterTObject(TPSPascalCompiler *c);

/* Methods exposed by TPSPascalCompiler */
extern TPascalCompiletimeClass *TPSPascalCompiler_AddClass(TPSPascalCompiler *c,
                                                           TPascalCompiletimeClass *inherits,
                                                           void *aClass);
extern TPascalCompiletimeClass *TPSPascalCompiler_AddClassN(TPSPascalCompiler *c,
                                                            TPascalCompiletimeClass *inherits,
                                                            const char *aClassName);
extern TPascalCompiletimeClass *TPSPascalCompiler_FindClass(TPSPascalCompiler *c,
                                                            const char *className);
extern TPascalInterface *TPSPascalCompiler_AddInterface(TPSPascalCompiler *c,
                                                        TPascalInterface *inherited,
                                                        const TGuid *guid,
                                                        const tbtString name);
extern TPascalInterface *TPSPascalCompiler_FindInterface(TPSPascalCompiler *c,
                                                         const tbtString name);
extern TPSRegProc *TPSPascalCompiler_AddDelphiFunction(TPSPascalCompiler *c,
                                                       const char *declaration);
extern TPSType *TPSPascalCompiler_AddType(TPSPascalCompiler *c,
                                          const char *name,
                                          TPSBaseType *baseType);
extern TPSType *TPSPascalCompiler_AddTypeS(TPSPascalCompiler *c,
                                           const char *name,
                                           const char *declaration);
extern TPSConstant *TPSPascalCompiler_AddConstant(TPSPascalCompiler *c,
                                                  const char *name,
                                                  TPSType *aType);
extern TPSConstant *TPSPascalCompiler_AddConstantN(TPSPascalCompiler *c,
                                                   const char *name,
                                                   const char *aTypeName);
extern TPSVar *TPSPascalCompiler_AddUsedVariable(TPSPascalCompiler *c,
                                                 const char *name,
                                                 TPSType *aType);
extern TPSVar *TPSPascalCompiler_AddUsedVariableN(TPSPascalCompiler *c,
                                                  const char *name,
                                                  const char *aTypeName);
extern TPSVar *TPSPascalCompiler_AddUsedPtrVariable(TPSPascalCompiler *c,
                                                    const char *name,
                                                    TPSType *aType);
extern TPSVar *TPSPascalCompiler_AddUsedPtrVariableN(TPSPascalCompiler *c,
                                                     const char *name,
                                                     const char *aTypeName);

/* ------------------------------------------------------------------ */
/* Callbacks used by PascalScript                                      */
/* ------------------------------------------------------------------ */
static bool OnNeedFile(TPSPreProcessor *sender,
                       const char *callingFilename,
                       char **fileName,
                       char **output) {
    (void)callingFilename;               /* not used in this simple port */
    TPascalCompiler *self = (TPascalCompiler *)sender; /* ID stored in sender */
    char *path = StrDup(*fileName);
    bool found = false;
    for (size_t i = 0; i < self->FSearchPaths.count && !found; ++i) {
        char *candidate = malloc(strlen(self->FSearchPaths.items[i]) + strlen(path) + 1);
        strcpy(candidate, self->FSearchPaths.items[i]);
        strcat(candidate, path);
        char *expanded = ExpandFileName(candidate);
        if (FileExists(expanded) &&
            strncmp(expanded, self->FSearchPaths.items[i],
                    strlen(self->FSearchPaths.items[i])) == 0) {
            free(*fileName);
            *fileName = expanded;          /* return the resolved path */
            /* load file content */
            FILE *f = fopen(expanded, "rb");
            if (f) {
                fseek(f, 0, SEEK_END);
                long sz = ftell(f);
                fseek(f, 0, SEEK_SET);
                char *buf = malloc(sz + 1);
                fread(buf, 1, sz, f);
                buf[sz] = '\0';
                fclose(f);
                *output = buf;
                found = true;
            }
        }
        free(candidate);
        if (!found) free(expanded);
    }
    free(path);
    return found;
}

static bool OnUses(TPSPascalCompiler *compiler, const char *name) {
    TPascalCompiler *self = (TPascalCompiler *)compiler; /* ID stored */
    if (strcmp(name, "SYSTEM") == 0) {
        if (self->FSupportDLLs)
            RegisterDll_Compiletime(compiler);
        SIRegisterTObject(compiler);
        if (self->FApi) {
            /* FApi is a TList of ICompilerAPI – here we treat it as a simple array */
            /* For simplicity we assume FApi points to a struct with count and items */
            /* Real implementation would iterate the list. */
        }
        return true;
    }
    char *unitFile = malloc(strlen(name) + 5);
    sprintf(unitFile, "%s.pas", name);
    bool found = false;
    for (size_t i = 0; i < self->FSearchPaths.count && !found; ++i) {
        char *candidate = malloc(strlen(self->FSearchPaths.items[i]) + strlen(unitFile) + 1);
        strcpy(candidate, self->FSearchPaths.items[i]);
        strcat(candidate, unitFile);
        char *expanded = ExpandFileName(candidate);
        if (FileExists(expanded)) {
            /* load and compile the unit */
            FILE *f = fopen(expanded, "rb");
            if (f) {
                fseek(f, 0, SEEK_END);
                long sz = ftell(f);
                fseek(f, 0, SEEK_SET);
                char *buf = malloc(sz + 1);
                fread(buf, 1, sz, f);
                buf[sz] = '\0';
                fclose(f);
                /* set as main file temporarily and compile */
                free(self->FMainFileName);
                self->FMainFileName = StrDup(name);
                free(self->FSource);
                self->FSource = buf;
                found = PascalCompiler_Compile(self);
                self->FCompiled = false;   /* keep compiling further units */
            }
        }
        free(candidate);
        free(expanded);
        if (found) break;
    }
    free(unitFile);
    return found;
}

static void CompTranslateLineInfo(TPSPascalCompiler *sender,
                                  uint32_t *pos, uint32_t *row,
                                  uint32_t *col, char **name) {
    TPascalCompiler *self = (TPascalCompiler *)sender;
    /* Delegate to pre‑processor line‑info helper – omitted for brevity */
    (void)self; (void)pos; (void)row; (void)col; (void)name;
}

/* ------------------------------------------------------------------ */
/* TPascalCompiler implementation                                      */
/* ------------------------------------------------------------------ */
TPascalCompiler *PascalCompiler_Create(const char *dir, const char *mainFile) {
    TPascalCompiler *self = calloc(1, sizeof(TPascalCompiler));
    if (!self) return NULL;

    StringList_Init(&self->FSearchPaths);
    StringList_Init(&self->FDefines);

    /* Load main file */
    char *fullPath = malloc(strlen(dir) + strlen(mainFile) + 1);
    strcpy(fullPath, dir);
    strcat(fullPath, mainFile);
    FILE *f = fopen(fullPath, "rb");
    if (f) {
        fseek(f, 0, SEEK_END);
        long sz = ftell(f);
        fseek(f, 0, SEEK_SET);
        self->FSource = malloc(sz + 1);
        fread(self->FSource, 1, sz, f);
        self->FSource[sz] = '\0';
        fclose(f);
    }
    free(fullPath);

    self->FMainFileName = StrDup(mainFile);
    self->FSupportDLLs = false;
    self->FCompiled = false;

    self->FPreProcessor = PSPreProcessor_Create();
    PSPreProcessor_SetID(self->FPreProcessor, self);
    PSPreProcessor_SetOnNeedFile(self->FPreProcessor, OnNeedFile);

    self->FCompiler = TPSPascalCompiler_Create();
    TPSPascalCompiler_SetID(self->FCompiler, self);
    TPSPascalCompiler_SetOnUses(self->FCompiler, OnUses);
    TPSPascalCompiler_SetOnTranslateLineInfo(self->FCompiler, CompTranslateLineInfo);

    /* Default compiler options */
    /* Assuming there are setter functions for these flags */
    /* TPSPascalCompiler_SetAllowNoBegin(self->FCompiler, true); */
    /* TPSPascalCompiler_SetAllowNoEnd(self->FCompiler, true);   */
    /* TPSPascalCompiler_SetAllowUnit(self->FCompiler, true);    */

    return self;
}

void PascalCompiler_Destroy(TPascalCompiler *self) {
    if (!self) return;
    free(self->FMainFileName);
    free(self->FSource);
    StringList_Free(&self->FSearchPaths);
    StringList_Free(&self->FDefines);
    if (self->FPreProcessor) PSPreProcessor_Free(self->FPreProcessor);
    if (self->FCompiler) TPSPascalCompiler_Free(self->FCompiler);
    free(self);
}

/* ------------------------------------------------------------------ */
/* Public methods                                                      */
/* ------------------------------------------------------------------ */
bool PascalCompiler_Compile(TPascalCompiler *self) {
    if (self->FCompiled) return true;

    PSPreProcessor_SetDefines(self->FPreProcessor, &self->FDefines);
    PSPreProcessor_SetMainFileName(self->FPreProcessor, self->FMainFileName);
    PSPreProcessor_SetMainFile(self->FPreProcessor, self->FSource);

    char *preprocessed = NULL;
    PSPreProcessor_PreProcess(self->FPreProcessor, self->FMainFileName, &preprocessed);
    if (!preprocessed) return false;

    bool ok = TPSPascalCompiler_Compile(self->FCompiler, preprocessed);
    free(preprocessed);

    if (ok) {
        PSPreProcessor_AdjustMessages(self->FPreProcessor, self->FCompiler);
        self->FCompiled = true;
    }
    return ok;
}

StringList *PascalCompiler_GetMessages(TPascalCompiler *self) {
    StringList *list = malloc(sizeof(StringList));
    StringList_Init(list);
    int cnt = TPSPascalCompiler_GetMsgCount(self->FCompiler);
    for (int i = 0; i < cnt; ++i) {
        const char *msg = TPSPascalCompiler_GetMsg(self->FCompiler, i);
        StringList_Add(list, msg);
    }
    return list;
}

void PascalCompiler_AddSearchPath(TPascalCompiler *self, const char *path) {
    char *expanded = ExpandFileName(path);
    char *withSlash = IncludeTrailingPathDelimiter(expanded);
    StringList_Add(&self->FSearchPaths, withSlash);
    free(expanded);
    free(withSlash);
}

/* ---- Class / Interface / Function / Type / Constant / Variable ---- */

TPascalCompiletimeClass *PascalCompiler_AddClass_Class(TPascalCompiler *self,
                                                      TPascalCompiletimeClass *inheritsFrom,
                                                      void *aClass) {
    return TPSPascalCompiler_AddClass(self->FCompiler, inheritsFrom, aClass);
}

TPascalCompiletimeClass *PascalCompiler_AddClass_Name(TPascalCompiler *self,
                                                     TPascalCompiletimeClass *inheritsFrom,
                                                     const char *aClassName) {
    return TPSPascalCompiler_AddClassN(self->FCompiler, inheritsFrom, aClassName);
}

TPascalCompiletimeClass *PascalCompiler_FindClass(TPascalCompiler *self,
                                                  const char *className) {
    return TPSPascalCompiler_FindClass(self->FCompiler, className);
}

TPascalInterface *PascalCompiler_AddInterface(TPascalCompiler *self,
                                              TPascalInterface *inheritedFrom,
                                              const TGuid *guid,
                                              const tbtString name) {
    return TPSPascalCompiler_AddInterface(self->FCompiler, inheritedFrom, guid, name);
}

TPascalInterface *PascalCompiler_FindInterface(TPascalCompiler *self,
                                               const tbtString name) {
    return TPSPascalCompiler_FindInterface(self->FCompiler, name);
}

TPSRegProc *PascalCompiler_AddFunction(TPascalCompiler *self,
                                       const char *declaration) {
    return TPSPascalCompiler_AddDelphiFunction(self->FCompiler, declaration);
}

TPSType *PascalCompiler_AddType_Base(TPascalCompiler *self,
                                     const char *name,
                                     TPSBaseType *baseType) {
    return TPSPascalCompiler_AddType(self->FCompiler, name, baseType);
}

TPSType *PascalCompiler_AddType_Decl(TPascalCompiler *self,
                                     const char *name,
                                     const char *declaration) {
    return TPSPascalCompiler_AddTypeS(self->FCompiler, name, declaration);
}

TPSConstant *PascalCompiler_AddConstant_Type(TPascalCompiler *self,
                                             const char *name,
                                             TPSType *aType) {
    return TPSPascalCompiler_AddConstant(self->FCompiler, name, aType);
}

TPSConstant *PascalCompiler_AddConstant_Name(TPascalCompiler *self,
                                             const char *name,
                                             const char *aTypeName) {
    return TPSPascalCompiler_AddConstantN(self->FCompiler, name, aTypeName);
}

TPSVar *PascalCompiler_AddVariable_Type(TPascalCompiler *self,
                                        const char *name,
                                        TPSType *aType) {
    return TPSPascalCompiler_AddUsedVariable(self->FCompiler, name, aType);
}

TPSVar *PascalCompiler_AddVariable_Name(TPascalCompiler *self,
                                        const char *name,
                                        const char *aTypeName) {
    return TPSPascalCompiler_AddUsedVariableN(self->FCompiler, name, aTypeName);
}

TPSVar *PascalCompiler_AddPtrVariable_Type(TPascalCompiler *self,
                                           const char *name,
                                           TPSType *aType) {
    TPSVar *v = TPSPascalCompiler_AddUsedPtrVariable(self->FCompiler, name, aType);
    if (v) {
        /* ExportName = Name – assuming struct has a field ExportName */
        /* v->ExportName = StrDup(v->Name); */
    }
    return v;
}

TPSVar *PascalCompiler_AddPtrVariable_Name(TPascalCompiler *self,
                                           const char *name,
                                           const char *aTypeName) {
    return TPSPascalCompiler_AddUsedPtrVariableN(self->FCompiler, name, aTypeName);
}

/* ---- Property accessors ---- */
const char *PascalCompiler_GetByteCode(TPascalCompiler *self) {
    static char *cached = NULL;
    if (!self->FCompiled) {
        if (!PascalCompiler_Compile(self)) return "";
    }
    if (cached) free(cached);
    TPSPascalCompiler_GetOutput(self->FCompiler, &cached);
    return cached ? cached : "";
}

const char *PascalCompiler_GetDebugCode(TPascalCompiler *self) {
    static char *cached = NULL;
    if (!self->FCompiled) {
        if (!PascalCompiler_Compile(self)) return "";
    }
    if (cached) free(cached);
    TPSPascalCompiler_GetDebugOutput(self->FCompiler, &cached);
    return cached ? cached : "";
}

StringList *PascalCompiler_GetDefines(TPascalCompiler *self) {
    return &self->FDefines;
}
void PascalCompiler_SetDefines(TPascalCompiler *self, StringList *defines) {
    StringList_Free(&self->FDefines);
    self->FDefines = *defines;   /* shallow copy – caller must not free original */
}

bool PascalCompiler_GetSupportDLLs(TPascalCompiler *self) {
    return self->FSupportDLLs;
}
void PascalCompiler_SetSupportDLLs(TPascalCompiler *self, bool value) {
    self->FSupportDLLs = value;
}

void *PascalCompiler_GetApi(TPascalCompiler *self) {
    return self->FApi;
}
void PascalCompiler_SetApi(TPascalCompiler *self, void *api) {
    self->FApi = api;
}

TPSPascalCompiler *PascalCompiler_GetCompiler(TPascalCompiler *self) {
    return self->FCompiler;
}