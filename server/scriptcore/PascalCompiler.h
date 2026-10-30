#ifndef PASCAL_COMPILER_H
#define PASCAL_COMPILER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* Forward declarations of PascalScript library types (assumed to be provided by the library) */
typedef struct TPSPreProcessor TPSPreProcessor;
typedef struct TPSPascalCompiler TPSPascalCompiler;
typedef struct TPSCompileTimeClass TPSCompileTimeClass;
typedef struct TPSInterface TPSInterface;
typedef struct TPSRegProc TPSRegProc;
typedef struct TPSBaseType TPSBaseType;
typedef struct TPSType TPSType;
typedef struct TPSConstant TPSConstant;
typedef struct TPSVar TPSVar;
typedef struct TGuid TGuid;
typedef char *tbtString;

/* Simple dynamic string list used by the compiler */
typedef struct StringList {
    char **items;
    size_t count;
    size_t capacity;
} StringList;

/* Opaque handle for the compiler API interface */
typedef struct ICompilerAPI ICompilerAPI;

/* Main compiler object */
typedef struct TPascalCompiler {
    char *FMainFileName;
    char *FSource;
    bool FCompiled;
    bool FSupportDLLs;
    StringList FSearchPaths;
    StringList FDefines;
    TPSPreProcessor *FPreProcessor;
    TPSPascalCompiler *FCompiler;
    void *FApi;               /* TList* in Pascal – here kept opaque */
} TPascalCompiler;

/* Constants mirrored from uPSCompiler */
enum {
    iptRW = 0,
    iptR  = 1,
    iptW  = 2
};

/* Type aliases for readability */
typedef TPSCompileTimeClass TPascalCompiletimeClass;
typedef TPSInterface TPascalInterface;

/* Function prototypes */
TPascalCompiler *PascalCompiler_Create(const char *dir, const char *mainFile);
void PascalCompiler_Destroy(TPascalCompiler *self);

bool PascalCompiler_Compile(TPascalCompiler *self);
StringList *PascalCompiler_GetMessages(TPascalCompiler *self);

void PascalCompiler_AddSearchPath(TPascalCompiler *self, const char *path);

TPascalCompiletimeClass *PascalCompiler_AddClass_Class(TPascalCompiler *self,
                                                      TPascalCompiletimeClass *inheritsFrom,
                                                      void *aClass);          /* TClass */
TPascalCompiletimeClass *PascalCompiler_AddClass_Name(TPascalCompiler *self,
                                                     TPascalCompiletimeClass *inheritsFrom,
                                                     const char *aClassName);

TPascalCompiletimeClass *PascalCompiler_FindClass(TPascalCompiler *self,
                                                  const char *className);

TPascalInterface *PascalCompiler_AddInterface(TPascalCompiler *self,
                                              TPascalInterface *inheritedFrom,
                                              const TGuid *guid,
                                              const tbtString name);

TPascalInterface *PascalCompiler_FindInterface(TPascalCompiler *self,
                                               const tbtString name);

TPSRegProc *PascalCompiler_AddFunction(TPascalCompiler *self,
                                       const char *declaration);

TPSType *PascalCompiler_AddType_Base(TPascalCompiler *self,
                                     const char *name,
                                     TPSBaseType *baseType);

TPSType *PascalCompiler_AddType_Decl(TPascalCompiler *self,
                                     const char *name,
                                     const char *declaration);

TPSConstant *PascalCompiler_AddConstant_Type(TPascalCompiler *self,
                                             const char *name,
                                             TPSType *aType);

TPSConstant *PascalCompiler_AddConstant_Name(TPascalCompiler *self,
                                             const char *name,
                                             const char *aTypeName);

TPSVar *PascalCompiler_AddVariable_Type(TPascalCompiler *self,
                                        const char *name,
                                        TPSType *aType);

TPSVar *PascalCompiler_AddVariable_Name(TPascalCompiler *self,
                                        const char *name,
                                        const char *aTypeName);

TPSVar *PascalCompiler_AddPtrVariable_Type(TPascalCompiler *self,
                                           const char *name,
                                           TPSType *aType);

TPSVar *PascalCompiler_AddPtrVariable_Name(TPascalCompiler *self,
                                           const char *name,
                                           const char *aTypeName);

/* Property accessors */
const char *PascalCompiler_GetByteCode(TPascalCompiler *self);
const char *PascalCompiler_GetDebugCode(TPascalCompiler *self);

/* Direct access to internal fields (mirroring Pascal properties) */
StringList *PascalCompiler_GetDefines(TPascalCompiler *self);
void PascalCompiler_SetDefines(TPascalCompiler *self, StringList *defines);

bool PascalCompiler_GetSupportDLLs(TPascalCompiler *self);
void PascalCompiler_SetSupportDLLs(TPascalCompiler *self, bool value);

void *PascalCompiler_GetApi(TPascalCompiler *self);
void PascalCompiler_SetApi(TPascalCompiler *self, void *api);

TPSPascalCompiler *PascalCompiler_GetCompiler(TPascalCompiler *self);

/* ICompilerAPI interface */
struct ICompilerAPI {
    void (*CompilerRegister)(ICompilerAPI *self, TPascalCompiler *compiler);
};

#endif /* PASCAL_COMPILER_H */