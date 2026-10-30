#include "Console.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

void console_init(PConsole self) {
    if (!self) return;
    memset(self, 0, sizeof(TConsole));
    self->CountMax = CONSOLE_MAX_MESSAGES;
    self->ScrollTickMax = 60;
    self->NewMessageWait = 30;
    self->AlphaCount = 255;
}

void console_scroll(PConsole self) {
    int x;
    if (!self || self->Count <= 0) { if (self) self->ScrollTick = 0; return; }
    for (x = 1; x < self->Count; x++) {
        free(self->TextMessage[x]);
        self->TextMessage[x] = self->TextMessage[x + 1] ? strdup(self->TextMessage[x + 1]) : NULL;
        self->TextMessageColor[x] = self->TextMessageColor[x + 1];
        self->NumMessage[x] = self->NumMessage[x + 1];
    }
    self->AlphaCount = 255;
    free(self->TextMessage[self->Count]);
    self->TextMessage[self->Count] = strdup("");
    self->NumMessage[self->Count] = 0;
    self->Count--;
    self->ScrollTick = 0;
}

void console_add(PConsole self, const char *what, unsigned int col) {
    if (!self || !what) return;
    self->Count++;
    self->ScrollTick = -self->NewMessageWait;
    self->TextMessage[self->Count] = strdup(what);
    self->TextMessageColor[self->Count] = col;
    self->NumMessage[self->Count] = -255;
    if (self->Count == 1) self->AlphaCount = 255;
    if (self->Count >= self->CountMax) console_scroll(self);
}

void console_num(PConsole self, const char *what, unsigned int col, int num) {
    if (!self || !what) return;
    self->Count++;
    self->ScrollTick = -self->NewMessageWait;
    self->TextMessage[self->Count] = strdup(what);
    self->TextMessageColor[self->Count] = col;
    self->NumMessage[self->Count] = num;
    if (self->Count >= self->CountMax) console_scroll(self);
}

void console_write(PConsole self, const char *what, unsigned int col) {
    if (!self || !what || what[0] == '\0') return;
    console_add(self, what, col);
}

char *console_get_contents(PConsole self) {
    char *res = NULL;
    size_t len = 0;
    int i;
    if (!self) return NULL;
    for (i = 1; i <= self->Count; i++) {
        if (self->TextMessage[i]) {
            size_t slen = strlen(self->TextMessage[i]) + 1;
            char *tmp = realloc(res, len + slen + 1);
            if (!tmp) { free(res); return NULL; }
            res = tmp;
            memcpy(res + len, self->TextMessage[i], slen);
            len += slen;
            res[len - 1] = '\n';
        }
    }
    if (res) res[len] = '\0';
    else res = strdup("");
    return res;
}
