#ifndef SCRIPTCOREFUNCTIONS_H
#define SCRIPTCOREFUNCTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Basic type definitions matching Pascal units */
typedef unsigned char Byte;
typedef unsigned int LongWord;
typedef int Integer;
typedef float Single;
typedef double Extended;
typedef int Boolean;   /* 0 = false, non‑zero = true */

/* Weapon style numbers (as used by the game engine) */
#define EAGLE_NUM       1
#define MP5_NUM         2
#define AK74_NUM        3
#define STEYRAUG_NUM    4
#define SPAS12_NUM      5
#define RUGER77_NUM     6
#define M79_NUM         7
#define BARRETT_NUM     8
#define M249_NUM        9
#define MINIGUN_NUM    10
#define COLT_NUM       11
#define KNIFE_NUM      12
#define CHAINSAW_NUM   13
#define LAW_NUM        14
#define NOWEAPON_NUM    0

/* Object style numbers (map to in‑game objects) */
#define OBJECT_DESERT_EAGLE       1
#define OBJECT_HK_MP5             2
#define OBJECT_AK74               3
#define OBJECT_STEYR_AUG          4
#define OBJECT_SPAS12             5
#define OBJECT_RUGER77            6
#define OBJECT_M79                7
#define OBJECT_BARRET_M82A1       8
#define OBJECT_MINIMI             9
#define OBJECT_MINIGUN           10
#define OBJECT_USSOCOM           11
#define OBJECT_COMBAT_KNIFE      12
#define OBJECT_CHAINSAW          13
#define OBJECT_LAW               14

/* Menu style numbers (used by the weapon‑selection menu) */
#define EAGLE        1
#define MP5          2
#define AK74         3
#define STEYRAUG     4
#define SPAS12       5
#define RUGER77      6
#define M79          7
#define BARRETT      8
#define M249         9
#define MINIGUN     10
#define COLT        11
#define KNIFE       12
#define CHAINSAW    13
#define LAW         14
#define NOWEAPON     0

/* ------------------------------------------------------------------ */
/* Function prototypes – these correspond to the functions registered */
/* in TCoreFunctionsAPI.CompilerRegister / RuntimeRegisterApi.       */
/* ------------------------------------------------------------------ */

void WriteLn(const char *Text);
int StrToInt(const char *Text);
void *iif(int Condition, void *IfTrue, void *IfFalse);
char *MD5(const char *A);
double ArcTan(double A);
double ExpFunc(double A);
double Ln(double A);
double Log10(double A);
double LogN(double A, double B);
int RandomInt(int Min, int Max);
LongWord RGB(Byte R, Byte G, Byte B);
float Distance(float X1, float X2, float Y1, float Y2);
int ExecRegExpr(const char *ARegExpr, const char *AInputStr);
int SplitRegExpr(const char *ARegExpr, const char *AInputStr,
                 char ***APieces, int *count);
char *ReplaceRegExpr(const char *ARegExpr, const char *AInputStr,
                     const char *AReplaceStr, int AUseSubstitution);
char *QuoteRegExprMetaChars(const char *AStr);
int RegExprSubExpressions(const char *ARegExpr,
                          char ***ASubExprs, int *count,
                          int AExtendedSyntax);
char *FormatFloat(const char *Format, double Value);

Byte weap2obj(Byte Style);
Byte menu2obj(Byte Style);
Byte obj2weap(Byte Style);
Byte obj2menu(Byte Style);
Byte weap2menu(Byte Style);
Byte menu2weap(Byte Style);

#ifdef __cplusplus
}
#endif

#endif /* SCRIPTCOREFUNCTIONS_H */