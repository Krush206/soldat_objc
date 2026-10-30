/* Cvar — C port */

#ifndef CVAR_H
#define CVAR_H

#define MAX_CVARS 1024

#define CVAR_IMMUTABLE (1<<0)
#define CVAR_ARCHIVE   (1<<1)
#define CVAR_SPONLY    (1<<2)
#define CVAR_NOTIFY    (1<<3)

typedef struct {
    char *name;
    int flags;
    char *description;
    char *error_message;
} TCvarBase;

typedef struct {
    TCvarBase base;
    int value;
    int default_value;
    int min_value;
    int max_value;
} TIntegerCvar;

typedef struct {
    TCvarBase base;
    float value;
    float default_value;
} TSingleCvar;

typedef struct {
    TCvarBase base;
    int value;
} TBooleanCvar;

typedef struct {
    TCvarBase base;
    char *value;
} TStringCvar;

/* Function declarations */
int CVar_GetInteger(const char *name);
const char *CVar_GetString(const char *name);
int CVar_GetFlags(const char *name);
void CVar_SetFlags(const char *name, int flags);
int CVar_Init(void);
void CVar_Shutdown(void);

#endif /* CVAR_H */
