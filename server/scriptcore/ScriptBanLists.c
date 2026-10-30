#include "ScriptBanLists.h"

/* Declarations for the underlying ban system (provided elsewhere) */
extern void AddBannedHW(const char *HW, const char *Reason, int Duration);
extern void AddBannedIP(const char *IP, const char *Reason, int Duration);
extern bool DelBannedHW(const char *HW);
extern bool DelBannedIP(const char *IP);
extern bool CheckBannedHW(const char *HW);
extern bool CheckBannedIP(const char *IP);
extern int FindBanHW(const char *HW);
extern int FindBan(const char *IP);          /* finds IP ban */
extern TBanHW *BannedHWList;                 /* array of hardware bans */
extern TBanIP *BannedIPList;                 /* array of IP bans */
extern int BannedHWListSize;                 /* number of elements in BannedHWList */
extern int BannedIPListSize;                 /* number of elements in BannedIPList */

/* ---- Public API implementations ---- */

void AddHWBan(const char *HW, const char *Reason, int Duration)
{
    AddBannedHW(HW, Reason, Duration);
}

void AddIPBan(const char *IP, const char *Reason, int Duration)
{
    AddBannedIP(IP, Reason, Duration);
}

bool DelHWBan(const char *HW)
{
    return DelBannedHW(HW);
}

bool DelIPBan(const char *IP)
{
    return DelBannedIP(IP);
}

bool IsBannedHW(const char *HW)
{
    return CheckBannedHW(HW);
}

bool IsBannedIP(const char *IP)
{
    return CheckBannedIP(IP);
}

int GetHWBanId(const char *HW)
{
    return FindBanHW(HW);
}

int GetIPBanId(const char *IP)
{
    return FindBan(IP);
}

TBanHW GetBannedHW(int Num)
{
    /* Assume Num is a valid index (0 <= Num < BannedHWListSize) */
    return BannedHWList[Num];
}

TBanIP GetBannedIP(int Num)
{
    /* Assume Num is a valid index (0 <= Num < BannedIPListSize) */
    return BannedIPList[Num];
}

int GetBannedHWCount(void)
{
    /* High() in Pascal returns the highest valid index.
       If the list is empty we return -1. */
    if (BannedHWListSize <= 0)
        return -1;
    return BannedHWListSize - 1;
}

int GetBannedIPCount(void)
{
    if (BannedIPListSize <= 0)
        return -1;
    return BannedIPListSize - 1;
}