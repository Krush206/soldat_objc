#ifndef SCRIPTCORE_INTERFACE_H
#define SCRIPTCORE_INTERFACE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Basic type mappings from Pascal */
typedef uint8_t  Byte;
typedef int32_t  Integer;
typedef uint32_t Cardinal;
typedef float    Single;
typedef double   Double;
typedef long double Extended;
typedef const char* String;          /* Pascal AnsiString/UnicodeString treated as C string */
typedef bool     Boolean;
typedef void*    Variant;            /* Opaque variant type */
typedef int32_t  Longint;
typedef uint32_t TColor;

/* Forward declarations of engine structures used by the API */
struct TPSScript;
struct TPascalCore;
struct TVector2 { float x, y; };
typedef struct TPSScript TPSScript;
typedef struct TPascalCore TPascalCore;
typedef struct TVector2 TVector2;

/* ScriptCore API functions */
String  ReadFile(String FileName, Boolean UseEncryption);
TColor  SRGB(Byte R, Byte G, Byte B);
Boolean ExistsFile(String FileName);
/* String ScriptGetURL(String URL);  -- commented out in Pascal */
String  Decrypt(String S, uint16_t Key);
String  Encrypt(String S, uint16_t Key);
String* MySplitStr(String Source, String Delimiter);   /* Returns dynamic array (TStringArray) */
String* SafeSplit(String Source, String Delimiter);
void    DrawText(Byte Id, String Text, Integer Delay, Longint Colour,
                 Single Scale, Integer X, Integer Y);
Boolean SetVal(TPSScript* Script, String Variable, Integer Value);
void    SetVariables(TPascalCore* ScriptCore, TPSScript* Script);
void    RegisterFunctions(TPascalCore* ScriptCore, TPSScript* Script);
Integer SArrayHigh(String* Arr);          /* High index of open array */
void    WriteLn(String Data);
Extended MyExp(Extended Number);
Integer SRand(Integer Min, Integer Max);
Integer SCreateBullet(Single X, Single Y, Single VelX, Single VelY,
                      Single HitM, Byte sStyle, Byte Owner);
void    SGiveBonus(Byte Id, Byte Bonus);
void    ServerModifier(String* Name, Single Value);
Boolean SRayCast(Single P1X, Single P1Y, Single P2X, Single P2Y,
                 Single* Distance, Single MaxDist);
Boolean SRayCastEx(Single P1X, Single P1Y, Single P2X, Single P2Y,
                   Single* Distance, Single MaxDist,
                   Boolean Player, Boolean Flag, Boolean Bullet,
                   Boolean Collider, Byte Team);
void    SetTeamScore(Byte Team, Integer Score);
Variant GetObjectStat(Byte Id, String ScriptStat);
Variant CrossFunc(Variant* Params, size_t ParamCount, String ProcName);
void    ResetTarget(Byte Id);
Boolean GetKeyPress(Byte Id, String ScriptKey);
Variant GetSpawnStat(Byte Id, String ScriptStat);
void    SetSpawnStat(Byte Id, String ScriptStat, Variant Value);
void    SBotChat(Byte Id, String Text);
Boolean RegExpMatch(String RegularExpression, String Source);
String  RegExpReplace(String RegularExpression, String Source,
                      String ReplaceText, Boolean UseSubstitution);
Extended MyRoundTo(Extended AValue, Integer ADigit);
String  MyDate(String Format);
void    DrawTextEx(Byte Id, Byte Num, String Text, Integer Delay,
                   Longint Colour, Single Scale, Integer X, Integer Y);
String  GetSystem(void);
String  SWeaponNameByNum(Integer Num);
Cardinal SGetTickCount(void);
String  IDToIP(Byte Id);
String  IDToHW(Byte Id);
void    SKickPlayer(Byte Num);
void    MyStartVoteKick(Byte Target, String Reason);
void    MyStartVoteMap(String Mapname, String Reason);
void    SBanPlayer(Byte Num, Integer Time);
void    SBanPlayerReason(Byte Num, Integer Time, String Reason);
Variant SCommand(String Cmd);
void    DoDamage(Byte Id, Integer Damage);
void    DoDamageBy(Byte Id, Byte Shooter, Integer Damage);
void    SForceWeapon(Byte A, Byte B, Byte C, Byte D);
void    SForceWeaponEx(Byte A, Byte B, Byte C, Byte D, Byte E);
void    SetScore(Byte Id, Integer Score);
Integer SCreateThing(Single X, Single Y, Byte BType);
void    KillThing(Integer Id);
void    GetPlayerXY(Byte Id, Single* X, Single* Y);
Variant GetPlayerStat(Byte Id, String ScriptStat);
void    GetFlagsXY(Single* BlueFlagX, Single* BlueFlagY,
                   Single* RedFlagX, Single* RedFlagY);
void    GetFlagsSpawnXY(Single* BlueFlagX, Single* BlueFlagY,
                        Single* RedFlagX, Single* RedFlagY);
void    SSLeep(Cardinal Milliseconds);
void    SSendStringToPlayer(Byte Id, String Text);
String  MyReadFile(String FileName);
Boolean WriteFile(String FileName, String SData);
Boolean AppendFile(String FileName, String Data);
Integer shell_exec(String Command);
String  ReadINI(String FileName, String Section, String Value, String Default);

#endif /* SCRIPTCORE_INTERFACE_H */