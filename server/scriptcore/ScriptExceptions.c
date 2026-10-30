#include "ScriptExceptions.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ---------- Helper functions ---------- */
static char *strdup_or_null(const char *s) {
    return s ? strdup(s) : NULL;
}

/* ---------- ScriptException implementation ---------- */
static char *ScriptException_get_message_impl(ScriptException *self) {
    if (!self) return NULL;
    if (!self->class_name) self->class_name = "ScriptException";
    if (!self->message) self->message = strdup("");
    size_t len = strlen("[") + strlen(self->class_name) + strlen("] ") + strlen(self->message) + 1;
    char *buf = malloc(len);
    if (!buf) return NULL;
    snprintf(buf, len, "[%s] %s", self->class_name, self->message);
    return buf;
}

static void ScriptException_set_message_impl(ScriptException *self, const char *msg) {
    if (!self) return;
    free(self->message);
    self->message = strdup_or_null(msg);
}

static void ScriptException_destroy_impl(ScriptException *self) {
    if (!self) return;
    free(self->message);
    free(self);
}

/* Constructor */
ScriptException *ScriptException_create(const char *msg) {
    ScriptException *e = calloc(1, sizeof(ScriptException));
    if (!e) return NULL;
    e->class_name = "ScriptException";
    e->message = strdup_or_null(msg);
    e->get_message = ScriptException_get_message_impl;
    e->set_message = ScriptException_set_message_impl;
    e->destroy = ScriptException_destroy_impl;
    return e;
}

/* Public wrappers */
void ScriptException_destroy(ScriptException *e) {
    if (e && e->destroy) e->destroy(e);
}

char *ScriptException_get_message(ScriptException *e) {
    if (e && e->get_message) return e->get_message(e);
    return NULL;
}

void ScriptException_set_message(ScriptException *e, const char *msg) {
    if (e && e->set_message) e->set_message(e, msg);
}

/* ---------- EScriptUnload ---------- */
EScriptUnload *EScriptUnload_create(const char *msg) {
    EScriptUnload *e = calloc(1, sizeof(EScriptUnload));
    if (!e) return NULL;
    /* initialise base */
    ScriptException *base = ScriptException_create(msg);
    if (!base) { free(e); return NULL; }
    e->base = *base;
    free(base);
    e->base.class_name = "EScriptUnload";
    return e;
}

void EScriptUnload_destroy(EScriptUnload *e) {
    if (!e) return;
    ScriptException_destroy(&e->base);
    free(e);
}

/* ---------- EScriptRecompile ---------- */
static void EScriptRecompile_destroy_impl(ScriptException *self) {
    /* base destroy is enough */
    ScriptException_destroy_impl(self);
}

EScriptRecompile *EScriptRecompile_create(const char *msg, bool force) {
    EScriptRecompile *e = calloc(1, sizeof(EScriptRecompile));
    if (!e) return NULL;
    ScriptException *base = ScriptException_create(msg);
    if (!base) { free(e); return NULL; }
    e->base = *base;
    free(base);
    e->base.class_name = "EScriptRecompile";
    e->force = force;
    /* override destroy if needed */
    e->base.destroy = EScriptRecompile_destroy_impl;
    return e;
}

void EScriptRecompile_destroy(EScriptRecompile *e) {
    if (!e) return;
    ScriptException_destroy(&e->base);
    free(e);
}

bool EScriptRecompile_get_force(const EScriptRecompile *e) {
    return e ? e->force : false;
}

/* ---------- EAccessDenied ---------- */
EAccessDenied *EAccessDenied_create(const char *msg) {
    EAccessDenied *e = calloc(1, sizeof(EAccessDenied));
    if (!e) return NULL;
    ScriptException *base = ScriptException_create(msg);
    if (!base) { free(e); return NULL; }
    e->base = *base;
    free(base);
    e->base.class_name = "EAccessDenied";
    return e;
}

void EAccessDenied_destroy(EAccessDenied *e) {
    if (!e) return;
    ScriptException_destroy(&e->base);
    free(e);
}

/* ---------- ENotImplemented ---------- */
ENotImplemented *ENotImplemented_create(const char *msg) {
    ENotImplemented *e = calloc(1, sizeof(ENotImplemented));
    if (!e) return NULL;
    ScriptException *base = ScriptException_create(msg);
    if (!base) { free(e); return NULL; }
    e->base = *base;
    free(base);
    e->base.class_name = "ENotImplemented";
    return e;
}

void ENotImplemented_destroy(ENotImplemented *e) {
    if (!e) return;
    ScriptException_destroy(&e->base);
    free(e);
}