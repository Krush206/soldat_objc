/* Anims — C port of shared/Anims.pas */

#ifndef ANIMS_H
#define ANIMS_H

#include "Vector.h"

#define MAX_POS_INDEX 20
#define MAX_FRAMES_INDEX 40

typedef struct TFrame {
    TVector3 Pos[MAX_POS_INDEX + 1]; /* Pascal 1..MAX_POS_INDEX */
} TFrame;

typedef struct TAnimation {
    int ID;
    TFrame Frames[MAX_FRAMES_INDEX + 1]; /* Pascal 1..MAX_FRAMES_INDEX */
    int NumFrames;
    int Speed, Count;
    int CurrFrame;
    bool Loop;
} TAnimation;

void TAnimation_DoAnimation(TAnimation *self);
void TAnimation_LoadFromFile(TAnimation *self, const char *filename);
int TAnimation_CheckSum(TAnimation *self);

void Anims_LoadAnimObjects(const char *moddir);

#endif /* ANIMS_H */