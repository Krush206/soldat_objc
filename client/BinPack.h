/* BinPack — C port of client/BinPack.pas */

#ifndef BINPACK_H
#define BINPACK_H

typedef struct TBPRect {
    int x, y;
    int w, h;
    void *Data;
} TBPRect;
typedef TBPRect *PBPRect;

/* Pascal dynamic array: pointer + count. */
typedef struct TBPRectArray {
    TBPRect *items;
    int count;
} TBPRectArray;

int BinPack_PackRects(int w, int h, TBPRectArray *rects);

#endif /* BINPACK_H */