/* Console — C port of shared/Console.pas */

#ifndef CONSOLE_H
#define CONSOLE_H

#define CONSOLE_MAX_MESSAGES 255

typedef struct {
    char *TextMessage[CONSOLE_MAX_MESSAGES + 1];
    unsigned int TextMessageColor[CONSOLE_MAX_MESSAGES + 1];
    int NumMessage[CONSOLE_MAX_MESSAGES + 1];
    int Count;
    int CountMax;
    int ScrollTick;
    int ScrollTickMax;
    int NewMessageWait;
    unsigned char AlphaCount;
} TConsole;

typedef TConsole *PConsole;

void console_init(PConsole self);
void console_scroll(PConsole self);
void console_write(PConsole self, const char *what, unsigned int col);
void console_add(PConsole self, const char *what, unsigned int col);
void console_num(PConsole self, const char *what, unsigned int col, int num);
char *console_get_contents(PConsole self);

#endif /* CONSOLE_H */
