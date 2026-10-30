#ifndef BANSYSTEM_H
#define BANSYSTEM_H

#include <stdbool.h>
#include <stddef.h>

/* Constant for permanent ban */
#define PERMANENT -1000

/* External functions used by this module */
extern void MainConsole_Console(const char *msg, int color);
extern void SaveTxtLists(void);
extern int CLIENT_MESSAGE_COLOR;

/* Ban structures */
typedef struct {
    char *ip;      /* IP address or mask, empty string means unused */
    int time;      /* Remaining time in seconds, PERMANENT for infinite */
    char *reason;  /* Reason for ban */
} BanIP;

typedef struct {
    char *hw;      /* Hardware ID or mask, empty string means unused */
    int time;
    char *reason;
} BanHW;

/* Public API */
void AddBannedIP(const char *ip, const char *reason, int duration);
bool DelBannedIP(const char *ip);
bool CheckBannedIP(const char *ip);
int FindBan(const char *ip);
void LoadBannedList(const char *filename);
void SaveBannedList(const char *filename);
void UpdateIPBanList(void);

void AddBannedHW(const char *hw, const char *reason, int duration);
bool DelBannedHW(const char *hw);
bool CheckBannedHW(const char *hw);
int FindBanHW(const char *hw);
void LoadBannedListHW(const char *filename);
void SaveBannedListHW(const char *filename);
void UpdateHWBanList(void);

#endif /* BANSYSTEM_H */