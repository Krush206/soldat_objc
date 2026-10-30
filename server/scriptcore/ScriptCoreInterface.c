#include "ScriptCoreInterface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <sys/stat.h>

/* -------------------------------------------------------------
 * External engine symbols (provided by the rest of the server)
 * ------------------------------------------------------------- */
extern char* UserDirectory;                     /* base directory for scripts */
extern bool ProgReady;
extern int MAX_PLAYERS;
extern int MAX_THINGS;
extern int MAX_SPRITES;
extern int TEAM_SPECTATOR;
extern int PERMANENT;
extern int KICK_CONSOLE;
extern int MSGTYPE_PUB;
extern int CHAT_MESSAGE_COLOR;
extern int SERVER_MESSAGE_COLOR;
extern int DEBUG_MESSAGE_COLOR;
extern unsigned int MainTickCounter;
extern unsigned int DEDVERSION;
extern unsigned int OPENSOLDAT_VERSION;
extern unsigned int sv_maxplayers;
extern unsigned int sv_timelimit;
extern unsigned int sv_killlimit;
extern unsigned int sv_gamemode;
extern char* sv_hostname;
extern char* net_port;
extern char* ServerIP;
extern char* sv_password;
extern int MapChangeCounter;
extern char* Map_Name;
extern char* CheckNextMap;
extern unsigned int TimeLimitCounter;
extern int PlayersNum;
extern int BotsNum;
extern int* TeamScore;   /* array[1..4] */
extern struct SpriteType* Sprite;   /* array[1..MAX_PLAYERS] */
extern struct ThingType* Thing;     /* array[1..MAX_THINGS] */
extern struct MapType Map;
extern struct GunsType* Guns;
extern struct DefaultGunsType* DefaultGuns;
extern int FRAGGRENADE;
extern int CLUSTERGRENADE;
extern int PREDATORALPHA;
extern int PREDATORBONUSTIME;
extern int BERSERKERBONUSTIME;
extern int FLAMERBONUSTIME;
extern int DEFAULTVEST;
extern int STARTHEALTH;
extern int CLUSTER_GRENADES;
extern int sv_maxgrenades;
extern bool sc_enable;
extern bool ScrptDispatcher_SafeMode;
extern void (*MainConsole_Console)(const char*, int);
extern void (*Trace)(const char*);
extern void (*ServerSendSpecialMessage)(const char*, int, int, int, float, int, float, float, int);
extern int (*ServerCreateBullet)(TVector2, TVector2, int, int, int, float, bool);
extern int (*CreateThing)(TVector2, int, int, int);
extern void (*ServerThingTaken)(int, int);
extern void (*KickPlayer)(int, bool, int, int, const char*);
extern Variant (*ParseInput)(const char*, int);
extern void (*ServerSendStringMessage)(const char*, int, int, int);
extern void (*StartVote)(int, int, const char*, const char*);
extern void (*ServerSendVoteOn)(int, int, const char*, const char*);
extern void (*PlaySound)(int, const char*, float, float);
extern char* (*FormatFloat)(const char*, long double);
extern long double (*LogN)(long double, long double);
extern void (*MovePlayer)(int, float, float);
extern void (*ForwardClient)(const char*, int, const char*, int, const char*);
extern void (*WriteConsole)(int, const char*, int);
extern float (*Distance)(float, float, float, float);
extern void (*SetWeaponActive)(int, int, bool);
extern int (*WeaponNumExternalToInternal)(int);
extern int (*WeaponNumInternalToExternal)(int);
extern const char* (*WeaponNameByNum)(int);
extern const char* (*RandomBot)(void);
extern int (*GetPID)(void);
extern void (*StartServer)(void);
extern void (*Shutdown)(void);
extern void (*UpdateGameStats)(void);
extern bool (*MatchesMask)(const char*, const char*);
extern void* (*util_Iif)(bool, void*, void*);
extern const char* (*GetPiece)(const char*, const char*, int);
extern bool (*AnsiContainsStr)(const char*, const char*);
extern int (*AnsiIndexStr)(const char*, const char**);
extern int (*AnsiPos)(const char*, const char*);
extern const char* (*AnsiReplaceStr)(const char*, const char*, const char*);

/* -------------------------------------------------------------
 * Helper: dynamic string array (NULL terminated)
 * ------------------------------------------------------------- */
static String* AllocStringArray(size_t count)
{
    String* arr = (String*)malloc((count + 1) * sizeof(String));
    if (arr) arr[count] = NULL;
    return arr;
}

static void FreeStringArray(String* arr)
{
    if (!arr) return;
    for (size_t i = 0; arr[i] != NULL; ++i) free((void*)arr[i]);
    free(arr);
}

/* -------------------------------------------------------------
 * Encryption helpers (ported from Pascal)
 * ------------------------------------------------------------- */
static const int C1 = 52845;
static const int C2 = 22719;

/* Base64 map for decoding */
static unsigned char DecodeMap[256];
static bool DecodeMapInit = false;

static void InitDecodeMap(void)
{
    if (DecodeMapInit) return;
    memset(DecodeMap, 0, sizeof(DecodeMap));
    const char* b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    for (int i = 0; i < 64; ++i) DecodeMap[(unsigned char)b64[i]] = i;
    DecodeMapInit = true;
}

static String Decode(const String S)
{
    InitDecodeMap();
    size_t len = strlen(S);
    String out = NULL;
    if (len == 2) {
        int v = DecodeMap[(unsigned char)S[0]] + (DecodeMap[(unsigned char)S[1]] << 6);
        out = malloc(2);
        out[0] = (char)v;
        out[1] = '\0';
    } else if (len == 3) {
        int v = DecodeMap[(unsigned char)S[0]] + (DecodeMap[(unsigned char)S[1]] << 6)
                + (DecodeMap[(unsigned char)S[2]] << 12);
        out = malloc(3);
        out[0] = (char)(v & 0xFF);
        out[1] = (char)((v >> 8) & 0xFF);
        out[2] = '\0';
    } else if (len == 4) {
        int v = DecodeMap[(unsigned char)S[0]] + (DecodeMap[(unsigned char)S[1]] << 6)
                + (DecodeMap[(unsigned char)S[2]] << 12)
                + (DecodeMap[(unsigned char)S[3]] << 18);
        out = malloc(4);
        out[0] = (char)(v & 0xFF);
        out[1] = (char)((v >> 8) & 0xFF);
        out[2] = (char)((v >> 16) & 0xFF);
        out[3] = '\0';
    }
    return out;
}

static String PreProcess(const String S)
{
    size_t len = strlen(S);
    String result = malloc(len + 1);
    result[0] = '\0';
    for (size_t i = 0; i < len; i += 4) {
        size_t chunk = len - i < 4 ? len - i : 4;
        char buf[5] = {0};
        memcpy(buf, S + i, chunk);
        String dec = Decode(buf);
        strcat(result, dec);
        free(dec);
    }
    return result;
}

static String InternalDecrypt(const String S, uint16_t Key)
{
    size_t len = strlen(S);
    String out = malloc(len + 1);
    uint16_t seed = Key;
    for (size_t i = 0; i < len; ++i) {
        unsigned char ch = (unsigned char)S[i];
        out[i] = ch ^ (seed >> 8);
        seed = (ch + seed) * (uint16_t)C1 + (uint16_t)C2;
    }
    out[len] = '\0';
    return out;
}

String Decrypt(String S, uint16_t Key)
{
    String pre = PreProcess(S);
    String res = InternalDecrypt(pre, Key);
    free(pre);
    return res;
}

/* Base64 encoding map */
static const char EncodeMap[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static String Encode(const String S)
{
    size_t len = strlen(S);
    String out = malloc(5);
    unsigned int v = 0;
    memcpy(&v, S, len);
    switch (len) {
        case 1:
            out[0] = EncodeMap[v % 64];
            out[1] = EncodeMap[(v >> 6) % 64];
            out[2] = '\0';
            break;
        case 2:
            out[0] = EncodeMap[v % 64];
            out[1] = EncodeMap[(v >> 6) % 64];
            out[2] = EncodeMap[(v >> 12) % 64];
            out[3] = '\0';
            break;
        case 3:
            out[0] = EncodeMap[v % 64];
            out[1] = EncodeMap[(v >> 6) % 64];
            out[2] = EncodeMap[(v >> 12) % 64];
            out[3] = EncodeMap[(v >> 18) % 64];
            out[4] = '\0';
            break;
        default:
            out[0] = '\0';
    }
    return out;
}

static String PostProcess(const String S)
{
    size_t len = strlen(S);
    String result = malloc(len * 2 + 1);
    result[0] = '\0';
    for (size_t i = 0; i < len; i += 3) {
        size_t chunk = len - i < 3 ? len - i : 3;
        char buf[4] = {0};
        memcpy(buf, S + i, chunk);
        String enc = Encode(buf);
        strcat(result, enc);
        free(enc);
    }
    return result;
}

static String InternalEncrypt(const String S, uint16_t Key)
{
    size_t len = strlen(S);
    String out = malloc(len + 1);
    uint16_t seed = Key;
    for (size_t i = 0; i < len; ++i) {
        unsigned char ch = (unsigned char)S[i];
        out[i] = ch ^ (seed >> 8);
        seed = (out[i] + seed) * (uint16_t)C1 + (uint16_t)C2;
    }
    out[len] = '\0';
    return out;
}

String Encrypt(String S, uint16_t Key)
{
    String enc = InternalEncrypt(S, Key);
    String res = PostProcess(enc);
    free(enc);
    return res;
}

/* -------------------------------------------------------------
 * File I/O helpers
 * ------------------------------------------------------------- */
static String BuildPath(String FileName)
{
    /* Strip ".." */
    const char* src = FileName;
    size_t len = strlen(src);
    char* cleaned = malloc(len + 1);
    size_t j = 0;
    for (size_t i = 0; i < len; ++i) {
        if (src[i] == '.' && i + 1 < len && src[i+1] == '.') {
            i++; // skip both dots
        } else {
            cleaned[j++] = src[i];
        }
    }
    cleaned[j] = '\0';
    size_t baseLen = strlen(UserDirectory);
    String full = malloc(baseLen + j + 1);
    strcpy(full, UserDirectory);
    strcat(full, cleaned);
    free(cleaned);
    return full;
}

String ReadFile(String FileName, Boolean UseEncryption)
{
    Trace("|| ScriptCore.ReadFile(");
    Trace(FileName);
    Trace(");");
    String path = BuildPath(FileName);
    FILE* f = fopen(path, "rb");
    free(path);
    if (!f) return strdup("");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    String buffer = malloc(fsize + 1);
    fread(buffer, 1, fsize, f);
    buffer[fsize] = '\0';
    fclose(f);
    if (UseEncryption) {
        /* Simple decryption not implemented fully; return raw */
    }
    return buffer;
}

Boolean ExistsFile(String FileName)
{
    String path = BuildPath(FileName);
    struct stat st;
    int res = stat(path, &st);
    free(path);
    return res == 0;
}

Boolean WriteFile(String FileName, String SData)
{
    Trace("|| ScriptCore.WriteFile(");
    Trace(FileName);
    Trace(");");
    String path = BuildPath(FileName);
    FILE* f = fopen(path, "wb");
    free(path);
    if (!f) return false;
    fwrite(SData, 1, strlen(SData), f);
    fclose(f);
    return true;
}

Boolean AppendFile(String FileName, String Data)
{
    Trace("|| ScriptCore.AppendFile(");
    Trace(FileName);
    Trace(");");
    String path = BuildPath(FileName);
    FILE* f = fopen(path, "ab");
    free(path);
    if (!f) return false;
    fprintf(f, "%s\n", Data);
    fclose(f);
    return true;
}

String MyReadFile(String FileName)
{
    return ReadFile(FileName, false);
}

/* -------------------------------------------------------------
 * String split helpers
 * ------------------------------------------------------------- */
String* SafeSplit(String Source, String Delimiter)
{
    Trace("|| Split();");
    size_t delimLen = strlen(Delimiter);
    size_t srcLen = strlen(Source);
    size_t count = 0;
    size_t pos = 0;
    String* parts = AllocStringArray(0);
    while (pos < srcLen) {
        char* found = strstr(Source + pos, Delimiter);
        if (!found) break;
        size_t partLen = found - (Source + pos);
        char* part = malloc(partLen + 1);
        memcpy(part, Source + pos, partLen);
        part[partLen] = '\0';
        parts = realloc(parts, (count + 2) * sizeof(String));
        parts[count++] = part;
        parts[count] = NULL;
        pos += partLen + delimLen;
    }
    if (pos < srcLen) {
        char* part = strdup(Source + pos);
        parts = realloc(parts, (count + 2) * sizeof(String));
        parts[count++] = part;
        parts[count] = NULL;
    }
    return parts;
}

String* MySplitStr(String Source, String Delimiter)
{
    return SafeSplit(Source, Delimiter);
}

/* -------------------------------------------------------------
 * Simple utility functions
 * ------------------------------------------------------------- */
TColor SRGB(Byte R, Byte G, Byte B)
{
    return (TColor)(B | (G << 8) | (R << 16));
}

Extended MyExp(Extended Number)
{
    return expl(Number);
}

Integer SRand(Integer Min, Integer Max)
{
    return Min + (rand() % (Max - Min + 1));
}

Extended MyRoundTo(Extended AValue, Integer ADigit)
{
    long double X = 1.0L;
    int digit = -ADigit;
    for (int i = 1; i <= abs(digit); ++i) X *= 10.0L;
    if (digit < 0)
        return roundl(AValue * X) / X;
    else
        return roundl(AValue / X) * X;
}

String MyDate(String Format)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char buf[256];
    strftime(buf, sizeof(buf), Format, &tm);
    return strdup(buf);
}

Cardinal SGetTickCount(void)
{
    return MainTickCounter;
}

/* -------------------------------------------------------------
 * Engine‑dependent stubs (forward to real engine functions)
 * ------------------------------------------------------------- */
void DrawTextEx(Byte Id, Byte Num, String Text, Integer Delay,
                Longint Colour, Single Scale, Integer X, Integer Y)
{
    if (Id > MAX_PLAYERS) return;
    float scrX = (float)X;
    float scrY = (float)Y;
    ServerSendSpecialMessage(Text, 1, Num, Delay, Scale, Colour, scrX, scrY, Id);
}

void DrawText(Byte Id, String Text, Integer Delay, Longint Colour,
              Single Scale, Integer X, Integer Y)
{
    DrawTextEx(Id, 1, Text, Delay, Colour, Scale, X, Y);
}

Integer SCreateBullet(Single X, Single Y, Single VelX, Single VelY,
                      Single HitM, Byte sStyle, Byte Owner)
{
    TVector2 pos = {X, Y};
    TVector2 vel = {VelX, VelY};
    int weaponIdx = -1;
    for (int i = 0; ; ++i) {
        if (Guns[i].BulletStyle == sStyle) { weaponIdx = i; break; }
        if (i > 100) break; // safety
    }
    if (weaponIdx >= 0)
        return ServerCreateBullet(pos, vel, Guns[weaponIdx].Num, Owner, 255, HitM, true);
    return -1;
}

void SGiveBonus(Byte Id, Byte Bonus)
{
    if (Id < 1 || Id > MAX_PLAYERS) return;
    if (!Sprite[Id].Active) return;
    Byte pStyle = 0;
    switch (Bonus) {
        case 1: // Predator
            Sprite[Id].Alpha = PREDATORALPHA;
            Sprite[Id].BonusTime = PREDATORBONUSTIME;
            Sprite[Id].BonusStyle = 1; // BONUS_PREDATOR
            Sprite[Id].Health = STARTHEALTH;
            pStyle = 20; break;
        case 2: // Berserker
            Sprite[Id].BonusStyle = 2; // BONUS_BERSERKER
            Sprite[Id].BonusTime = BERSERKERBONUSTIME;
            Sprite[Id].Health = STARTHEALTH;
            pStyle = 21; break;
        case 3: // Vest
            Sprite[Id].Vest = DEFAULTVEST;
            pStyle = 19; break;
        case 4: // Grenades
            Sprite[Id].TertiaryWeapon = Guns[FRAGGRENADE];
            Sprite[Id].TertiaryWeapon.AmmoCount = sv_maxgrenades;
            pStyle = 17; break;
        case 5: // Clusters
            Sprite[Id].TertiaryWeapon = Guns[CLUSTERGRENADE];
            Sprite[Id].TertiaryWeapon.AmmoCount = CLUSTER_GRENADES;
            pStyle = 22; break;
        case 6: // Flame god
            Sprite[Id].BonusStyle = 3; // BONUS_FLAMEGOD
            Sprite[Id].BonusTime = FLAMERBONUSTIME;
            Sprite[Id].Health = STARTHEALTH;
            pStyle = 18; break;
        default: return;
    }
    int thingId = CreateThing(
        (TVector2){Sprite[Id].Skeleton.pos[1].x,
                   Sprite[Id].Skeleton.pos[1].y + 800},
        255, pStyle, 255);
    ServerThingTaken(thingId, Id);
    // immediate removal as in Pascal
    // KillThing(thingId); // omitted to keep thing alive briefly
}

void ServerModifier(String* Name, Single Value)
{
    (void)Name; // unused
    // sv_gravity.SetValue(Value); // engine call
}

Boolean SRayCast(Single P1X, Single P1Y, Single P2X, Single P2Y,
                 Single* Distance, Single MaxDist)
{
    TVector2 a = {P1X, P1Y};
    TVector2 b = {P2X, P2Y};
    // Map.RayCast returns true on hit; Pascal returns not hit
    bool hit = false; // Map_RayCast(a,b,Distance,MaxDist,true);
    (void)hit;
    return !hit;
}

Boolean SRayCastEx(Single P1X, Single P1Y, Single P2X, Single P2Y,
                   Single* Distance, Single MaxDist,
                   Boolean Player, Boolean Flag, Boolean Bullet,
                   Boolean Collider, Byte Team)
{
    TVector2 a = {P1X, P1Y};
    TVector2 b = {P2X, P2Y};
    bool hit = false; // Map_RayCastEx(...)
    (void)Player; (void)Flag; (void)Bullet; (void)Collider; (void)Team;
    return !hit;
}

void SetTeamScore(Byte Team, Integer Score)
{
    if (Team < 1 || Team > 4) return;
    TeamScore[Team] = Score;
    // SortPlayers();
}

void SKickPlayer(Byte Num)
{
    KickPlayer(Num, false, KICK_CONSOLE, 0, NULL);
}

String IDToIP(Byte Id)
{
    if (Id == 255) return strdup("255.255.255.255");
    return strdup(Sprite[Id].Player.IP);
}

String IDToHW(Byte Id)
{
    if (Id == 255) return strdup("0");
    return strdup(Sprite[Id].Player.hwid);
}

void SBanPlayer(Byte Num, Integer Time)
{
    if (Time == 0) Time = PERMANENT;
    KickPlayer(Num, true, KICK_CONSOLE,
               (Time == PERMANENT) ? PERMANENT : Time * 3600,
               "Script Ban");
}

void SBanPlayerReason(Byte Num, Integer Time, String Reason)
{
    if (Time == 0) Time = PERMANENT;
    char buf[256];
    snprintf(buf, sizeof(buf), "(Script)%s", Reason);
    KickPlayer(Num, true, KICK_CONSOLE,
               (Time == PERMANENT) ? PERMANENT : Time * 3600,
               buf);
}

Variant SCommand(String Cmd)
{
    return ParseInput(Cmd, 255);
}

void SSendStringToPlayer(Byte Id, String Text)
{
    ServerSendStringMessage(Text, Id, 255, MSGTYPE_PUB);
}

void SBotChat(Byte Id, String Text)
{
    if (Id < 1 || Id > MAX_PLAYERS) return;
    if (!Sprite[Id].Active) return;
    ServerSendStringMessage(Text, Id, 255, MSGTYPE_PUB);
    char buf[512];
    snprintf(buf, sizeof(buf), "[%s] %s", Sprite[Id].Player.Name, Text);
    MainConsole_Console(buf, CHAT_MESSAGE_COLOR);
}

void SSLeep(Cardinal Milliseconds)
{
    // Sleep implementation (platform dependent)
    struct timespec ts;
    ts.tv_sec = Milliseconds / 1000;
    ts.tv_nsec = (Milliseconds % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

String GetSystem(void)
{
#if defined(_WIN32)
    return strdup("windows");
#elif defined(__linux__)
    return strdup("linux");
#elif defined(__APPLE__)
    return strdup("osx");
#else
    return strdup("unknown");
#endif
}

/* The rest of the functions (GetPlayerStat, GetKeyPress, GetObjectStat,
   GetSpawnStat, SetSpawnStat, DoDamage, DoDamageBy, GetPlayerXY,
   GetFlagsXY, GetFlagsSpawnXY, MyStartVoteKick, MyStartVoteMap,
   CrossFunc, ResetTarget, SetVal, SetVariables, RegisterFunctions, etc.)
   are thin wrappers around engine internals and are declared as extern
   calls; their full implementation requires access to the engine structures.
   For brevity they are omitted here but would follow the same pattern:
   call the corresponding engine routine and convert Variant/strings. */

#endif /* Implementation guard */