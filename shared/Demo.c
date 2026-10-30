/* Demo — C port of shared/Demo.pas */

#include "Demo.h"
#include <string.h>
#include <stdlib.h>

TDemoRecorder *TDemoRecorder_Create(void)
{
    return NULL;  /* TODO: implement */
}

void TDemoRecorder_Free(TDemoRecorder *recorder)
{
    (void)recorder;
    /* TODO: implement */
}

bool TDemoRecorder_StartRecord(TDemoRecorder *recorder, const char *filename)
{
    (void)recorder;
    (void)filename;
    return false;  /* TODO: implement */
}

void TDemoRecorder_StopRecord(TDemoRecorder *recorder)
{
    (void)recorder;
    /* TODO: implement */
}

void TDemoRecorder_SaveRecord(TDemoRecorder *recorder, void *r, int size)
{
    (void)recorder;
    (void)r;
    (void)size;
    /* TODO: implement */
}

void TDemoRecorder_SaveNextFrame(TDemoRecorder *recorder)
{
    (void)recorder;
    /* TODO: implement */
}

void TDemoRecorder_SavePosition(TDemoRecorder *recorder)
{
    (void)recorder;
    /* TODO: implement */
}

int TDemoRecorder_TicksNum(TDemoRecorder *recorder)
{
    (void)recorder;
    return 0;  /* TODO: implement */
}

/* TODO: implement remaining Demo functions */
