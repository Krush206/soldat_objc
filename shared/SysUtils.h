/* SysUtils — Objective-C port of the FreePascal SysUtils/StrUtils/Math
 * functionality used by OpenSoldat. Plain C functions, no Foundation. */

#ifndef SYSUTILS_H
#define SYSUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

/* ------------------------------------------------------------------ */
/* Basic Pascal types                                                  */
/* ------------------------------------------------------------------ */

typedef unsigned int TColor;
typedef double TDateTime;

#define MaxInt 2147483647
#define MinInt (-2147483647 - 1)
#define MaxSingle 3.402823466e+38f
#define MinSingle 1.175494351e-38f
#define MaxWord 65535

/* ------------------------------------------------------------------ */
/* String helpers                                                      */
/* ------------------------------------------------------------------ */

/* Pascal strings are 1-based; these helpers use 0-based C semantics but
 * the same names. Copy(S, start, count) in Pascal starts at 1. */

/* Copy(Source, Start, Length) — Pascal-style 1-based start. */
char *Copy(const char *source, int start, int length);
/* Pos(SubStr, S) — returns 1-based position, 0 when not found. */
int Pos(const char *substr, const char *s);
int PosEx(const char *substr, const char *s, int offset);
/* Delete(S, Index, Count) — in-place Pascal-style delete, 1-based. */
void Delete(char *s, int index, int count);
/* Insert(Source, S, Index) — in-place Pascal-style insert, 1-based. */
void Insert(const char *source, char *s, int index);

char *UpperCase(const char *s);
char *LowerCase(const char *s);
char *Trim(const char *s);
char *TrimLeft(const char *s);
char *TrimRight(const char *s);

/* StringReplace with a small set of flags: rfReplaceAll = 1, rfIgnoreCase = 2 */
#define RF_REPLACEALL 1
#define RF_IGNORECASE 2
char *StringReplace(const char *s, const char *oldstr, const char *newstr,
                    int flags);

char *LeftStr(const char *s, int n);
char *RightStr(const char *s, int n);
char *MidStr(const char *s, int start, int count);

int LastDelimiter(const char *delimiters, const char *s);

/* Concatenation and formatting (results are never freed; Boehm GC). */
char *strconcat(const char *a, const char *b);
char *strconcat3(const char *a, const char *b, const char *c);
char *strdup2(const char *s); /* NULL-safe strdup */

char *Format(const char *fmt, ...);
char *IntToStr(int value);
char *IntToStr64(int64_t value);
char *IntToHex(unsigned int value, int digits);
char *IntToHex64(uint64_t value, int digits);
char *BoolToStr(int value, const char *trueS, const char *falseS);

int StrToInt(const char *s);
int StrToIntDef(const char *s, int def);
double StrToFloat(const char *s);
int64_t StrToInt64Def(const char *s, int64_t def);
unsigned int StrToHexDef(const char *s, unsigned int def);
double StrToFloatDef(const char *s, double def);
float StrToFloatDefF(const char *s, float def);
int StrToBoolDef(const char *s, int def);

char *FloatToStr(double value);
char *FloatToStrF(double value, const char *format); /* FormatFloat subset */
/* FormatFloat('0.##', v), ('#,##0', v), ('0.######', v) supported. */

/* Pascal String.StartsWith / EndsWith / Split helpers. */
int StartsWith(const char *s, const char *prefix);
int EndsWith(const char *s, const char *suffix);
/* Splits s on delim into a malloc'd array; returns count via *count. */
char **SplitString(const char *s, const char *delim, int *count);
/* Split on first occurrence only: returns left part, rest goes to *right. */
char *SplitFirst(const char *s, const char *delim, char **right);

/* QuotedStr: single quotes doubled and wrapped. */
char *QuotedStr(const char *s);

/* ------------------------------------------------------------------ */
/* Math helpers (Pascal Math/System unit)                              */
/* ------------------------------------------------------------------ */

#define Min(a, b) ((a) < (b) ? (a) : (b))
#define Max(a, b) ((a) > (b) ? (a) : (b))
static inline int IsZero(double v) { return v > -0.000001 && v < 0.000001; }
static inline int IsSingleZero(float v) { return v > -0.000001f && v < 0.000001f; }
#define IfThen(cond, a, b) ((cond) ? (a) : (b))
#define Iif(cond, a, b) ((cond) ? (a) : (b))
#define Sqr(x) ((x) * (x))
#define InRange(v, a, b) ((v) >= (a) && (v) <= (b))
#define Power2(x) pow(2, x)

#define Trunc(x) ((int)(x))
#define Frac(x) ((x) - (double)(int64_t)(x))
#define Ceil(x) ((int)ceil(x))
#define Floor(x) ((int)floor(x))
#define Sgn(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))

int Random(int range);          /* 0..range-1, Pascal Random(n) */
void Randomize(void);
void Sleep(int milliseconds);

/* Pascal set-of helpers on unsigned char/word/int bitmasks. */
#define InSet(bitmask, bit) (((bitmask) & (bit)) != 0)
#define InSet64(bitmask, bit) (((bitmask) & (bit)) != 0)
#define IncludeFlag(bitmask, bit) ((bitmask) |= (bit))
#define ExcludeFlag(bitmask, bit) ((bitmask) &= ~(bit))

/* ------------------------------------------------------------------ */
/* File system helpers                                                 */
/* ------------------------------------------------------------------ */

int FileExists(const char *filename);
int DirectoryExists(const char *dirname);
int CreateDir(const char *dirname);
int DeleteFile(const char *filename);
long FileSizeOf(const char *filename); /* -1 when missing */

/* TSearchRec subset used by the game. */
#define faReadOnly 0x01
#define faHidden 0x02
#define faSysFile 0x04
#define faDirectory 0x10
#define faArchive 0x20
#define faAnyFile 0x3f

typedef struct {
	int Attr;
	int64_t Size;
	unsigned int Mode;
	long Time;      /* mtime */
	char Name[1024];
	/* Internal state for FindFirst/FindNext/FindClose. */
	void *_dir;
	char _dirname[1024];
	char _pattern[256];
} TSearchRec;

int FindFirst(const char *pattern, int attr, TSearchRec *rec);
int FindNext(TSearchRec *rec);
void FindClose(TSearchRec *rec);

char *ExtractFileName(const char *filename);
char *ExtractFilePath(const char *filename);
char *ExtractFileExt(const char *filename);
char *ChangeFileExt(const char *filename, const char *ext);
char *IncludeTrailingPathDelimiter(const char *path);
char *ExcludeTrailingPathDelimiter(const char *path);

int64_t FileAge(const char *filename); /* -1 when missing, else mtime */

/* ------------------------------------------------------------------ */
/* Date/time helpers (Pascal SysUtils DateUtils subset)                */
/* ------------------------------------------------------------------ */

TDateTime Now(void);
TDateTime Date(void);
TDateTime Time(void);
/* Supported tokens: yy yyyy mm dd hh nn ss (Pascal "nn" = minutes). */
char *FormatDateTime(const char *format, TDateTime datetime);
int DayOfWeek(TDateTime datetime); /* 1 = Sunday .. 7 = Saturday */

#endif /* SYSUTILS_H */
