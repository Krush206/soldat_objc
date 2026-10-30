#ifndef SCRIPTBANLISTS_H
#define SCRIPTBANLISTS_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Ban record structures */
typedef struct {
    char *HW;        /* Hardware identifier */
    int Time;        /* Ban timestamp / duration */
    char *Reason;    /* Ban reason */
} TBanHW;

typedef struct {
    char *IP;        /* IP address */
    int Time;        /* Ban timestamp / duration */
    char *Reason;    /* Ban reason */
} TBanIP;

/* Public API */
void AddHWBan(const char *HW, const char *Reason, int Duration);
void AddIPBan(const char *IP, const char *Reason, int Duration);
bool DelHWBan(const char *HW);
bool DelIPBan(const char *IP);
bool IsBannedHW(const char *HW);
bool IsBannedIP(const char *IP);
int GetHWBanId(const char *HW);
int GetIPBanId(const char *IP);
TBanHW GetBannedHW(int Num);
TBanIP GetBannedIP(int Num);
int GetBannedHWCount(void);
int GetBannedIPCount(void);

#ifdef __cplusplus
}
#endif

#endif /* SCRIPTBANLISTS_H */