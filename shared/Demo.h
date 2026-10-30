/* Demo — C port of shared/Demo.pas (client branch) */

#ifndef DEMO_H
#define DEMO_H

#include "SysUtils.h"

#define DEMO_MAGIC "SOLDEM"

typedef struct TDemoHeader {
    char Header[7];        /* Pascal array[1..6] of Char */
    unsigned short Version;
    char MapName[161];      /* Pascal array[0..160] of Char */
    int StartDate;
    int TicksNum;
} TDemoHeader;

typedef struct TDemo {
    unsigned char *FDemoFileData;
    int FDemoFileSize;
    int FDemoFilePos;
    TDemoHeader FDemoHeader;
    bool FActive;
    char *FName;
    unsigned char FOldCam;
} TDemo;

typedef struct TDemoRecorder {
    TDemo base;
    int FTicksNum;
} TDemoRecorder;

typedef struct TDemoPlayer {
    TDemo base;
    int FSkipTo;
} TDemoPlayer;

TDemo *TDemo_Create(void);
void TDemo_Free(TDemo *demo);
bool TDemo_Active(TDemo *demo);
void TDemo_SetActive(TDemo *demo, bool active);
const char *TDemo_Name(TDemo *demo);
TDemoHeader TDemo_Header(TDemo *demo);

TDemoRecorder *TDemoRecorder_Create(void);
void TDemoRecorder_Free(TDemoRecorder *recorder);
bool TDemoRecorder_StartRecord(TDemoRecorder *recorder, const char *filename);
void TDemoRecorder_StopRecord(TDemoRecorder *recorder);
void TDemoRecorder_SaveRecord(TDemoRecorder *recorder, void *r, int size);
void TDemoRecorder_SaveNextFrame(TDemoRecorder *recorder);
void TDemoRecorder_SavePosition(TDemoRecorder *recorder);
int TDemoRecorder_TicksNum(TDemoRecorder *recorder);

TDemoPlayer *TDemoPlayer_Create(void);
void TDemoPlayer_Free(TDemoPlayer *player);
bool TDemoPlayer_OpenDemo(TDemoPlayer *player, const char *filename);
void TDemoPlayer_StopDemo(TDemoPlayer *player);
void TDemoPlayer_ProcessDemo(TDemoPlayer *player);
void TDemoPlayer_Position(TDemoPlayer *player, int ticks);
int TDemoPlayer_SkipTo(TDemoPlayer *player);

extern struct TDemoRecorder *DemoRecorder;
extern struct TDemoPlayer *DemoPlayer;
extern unsigned short RSize;
extern unsigned char FreeCam;
extern unsigned char NoTexts;

#endif /* DEMO_H */