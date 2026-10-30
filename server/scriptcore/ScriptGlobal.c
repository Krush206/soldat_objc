/*
 * ScriptGlobal.c
 * Implementation of ScriptGlobal and ScriptGlobalAPI
 */

#include "ScriptGlobal.h"
#include <stdlib.h>
#include <string.h>

/* Global format settings similar to Delphi DefaultFormatSettings */
static struct {
    char dateSeparator;
    char* shortDateFormat;
} defaultFormatSettings = {
    .dateSeparator = '/',
    .shortDateFormat = "yyyy-MM-dd"
};

/* ScriptGlobal implementation */
ScriptGlobal* ScriptGlobal_Create(void) {
    ScriptGlobal* self = (ScriptGlobal*)malloc(sizeof(ScriptGlobal));
    if (!self) return NULL;
    self->dateSeparator = defaultFormatSettings.dateSeparator;
    self->shortDateFormat = strdup(defaultFormatSettings.shortDateFormat);
    return self;
}

void ScriptGlobal_Destroy(ScriptGlobal* self) {
    if (!self) return;
    free(self->shortDateFormat);
    free(self);
}

char ScriptGlobal_GetDateSeparator(ScriptGlobal* self) {
    return defaultFormatSettings.dateSeparator;
}

void ScriptGlobal_SetDateSeparator(ScriptGlobal* self, char separator) {
    defaultFormatSettings.dateSeparator = separator;
    if (self) self->dateSeparator = separator;
}

const char* ScriptGlobal_GetShortDateFormat(ScriptGlobal* self) {
    return defaultFormatSettings.shortDateFormat;
}

void ScriptGlobal_SetShortDateFormat(ScriptGlobal* self, const char* format) {
    if (defaultFormatSettings.shortDateFormat) free(defaultFormatSettings.shortDateFormat);
    defaultFormatSettings.shortDateFormat = strdup(format);
    if (self) {
        free(self->shortDateFormat);
        self->shortDateFormat = strdup(format);
    }
}

/* ScriptGlobalAPI implementation */
ScriptGlobalAPI* ScriptGlobalAPI_Create(void* script) {
    (void)script; /* script parameter not used in this minimal implementation */
    ScriptGlobalAPI* api = (ScriptGlobalAPI*)malloc(sizeof(ScriptGlobalAPI));
    if (!api) return NULL;
    api->global = ScriptGlobal_Create();
    return api;
}

void ScriptGlobalAPI_Destroy(ScriptGlobalAPI* api) {
    if (!api) return;
    ScriptGlobal_Destroy(api->global);
    free(api);
}

/* Stubs for compiler/runtime registration; actual registration depends on the Pascal compiler/runtime API */
void ScriptGlobalAPI_CompilerRegister(ScriptGlobalAPI* api, void* compiler) {
    (void)api; (void)compiler;
    /* In real implementation, register class TGlobal with properties DateSeparator (Char) and ShortDateFormat (string) */
}

void ScriptGlobalAPI_RuntimeRegisterApi(ScriptGlobalAPI* api, void* exec) {
    (void)api; (void)exec;
    /* Register property helpers for DateSeparator and ShortDateFormat */
}

void ScriptGlobalAPI_RuntimeRegisterVariables(ScriptGlobalAPI* api, void* exec) {
    (void)api; (void)exec;
    /* Set global variable 'Global' pointing to api->global */
}