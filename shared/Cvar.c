/* Cvar — C port of shared/Cvar.pas
 * Console variable system for OpenSoldat.
 * Plain C implementation, no Foundation/Classes frameworks.
 */

#include "Cvar.h"
#include "Util.h"
#include "Constants.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* --------------------------------------------------------------- internal */

#define CVAR_MAX 2048

typedef struct {
    char name[64];
    char value[256];
    int  flags;
    char description[256];
} TCvarEntry;

/* Global cvar store */
static struct {
    TCvarEntry cvars[CVAR_MAX];
    int        count;
} g_cvars;

/* --------------------------------------------------------------- public API */

int CVar_Init(void)
{
    if (g_cvars.count > 0) return 0; /* already initialised */
    g_cvars.count = 0;
    return 0;
}

void CVar_Shutdown(void)
{
    g_cvars.count = 0;
}

/* Find a cvar by name; returns index or -1 */
int CVar_Find(const char *name)
{
    if (!name) return -1;
    for (int i = 0; i < g_cvars.count; i++)
        if (strcmp(g_cvars.cvars[i].name, name) == 0) return i;
    return -1;
}

/* Register a new cvar; returns 0 on success, -1 if full or duplicate */
int CVar_Register(const char *name, const char *default_value,
                  int flags, const char *description)
{
    if (!name || !default_value) return -1;
    int idx = CVar_Find(name);
    if (idx >= 0) return -1; /* already registered */

    if (g_cvars.count >= CVAR_MAX) return -1;

    TCvarEntry *e = &g_cvars.cvars[g_cvars.count++];
    strncpy(e->name, name, sizeof(e->name) - 1);
    e->name[sizeof(e->name) - 1] = '\0';
    strncpy(e->value, default_value, sizeof(e->value) - 1);
    e->value[sizeof(e->value) - 1] = '\0';
    e->flags = flags;
    if (description) {
        strncpy(e->description, description, sizeof(e->description) - 1);
    } else {
        e->description[0] = '\0';
    }
    e->description[sizeof(e->description) - 1] = '\0';
    return 0;
}

/* Get cvar value as string */
const char *CVar_GetString(const char *name)
{
    int idx = CVar_Find(name);
    if (idx >= 0) return g_cvars.cvars[idx].value;
    return NULL;
}

/* Get cvar value as integer */
int CVar_GetInteger(const char *name)
{
    const char *v = CVar_GetString(name);
    if (!v) return 0;
    return atoi(v);
}

/* Set cvar value from string; returns 0 on success, -1 on failure */
int CVar_SetString(const char *name, const char *value)
{
    if (!name || !value) return -1;
    int idx = CVar_Find(name);
    if (idx < 0) return -1; /* not registered */
    strncpy(g_cvars.cvars[idx].value, value, sizeof(g_cvars.cvars[idx].value) - 1);
    g_cvars.cvars[idx].value[sizeof(g_cvars.cvars[idx].value) - 1] = '\0';
    return 0;
}

/* Set cvar value from integer */
int CVar_SetInteger(const char *name, int value)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%d", value);
    return CVar_SetString(name, buf);
}

/* Get cvar flags */
int CVar_GetFlags(const char *name)
{
    int idx = CVar_Find(name);
    if (idx < 0) return 0;
    return g_cvars.cvars[idx].flags;
}

/* Set cvar flags */
void CVar_SetFlags(const char *name, int flags)
{
    int idx = CVar_Find(name);
    if (idx >= 0) g_cvars.cvars[idx].flags = flags;
}