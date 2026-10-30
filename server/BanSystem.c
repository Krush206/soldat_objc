#include "BanSystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fnmatch.h>
#include <ctype.h>
#include <limits.h>

/* Dynamic arrays for bans */
static BanIP *bannedIPList = NULL;
static size_t bannedIPCount = 0;
static size_t bannedIPCapacity = 0;

static BanHW *bannedHWList = NULL;
static size_t bannedHWCount = 0;
static size_t bannedHWCapacity = 0;

/* Last ban strings (mirrors Pascal globals) */
static char *lastBan = NULL;
static char *lastBanHW = NULL;

/* Helper: ensure capacity for IP list */
static void ensureIPCapacity(size_t needed) {
    if (bannedIPCapacity >= needed) return;
    size_t newCap = bannedIPCapacity ? bannedIPCapacity * 2 : 4;
    while (newCap < needed) newCap *= 2;
    BanIP *newList = realloc(bannedIPList, newCap * sizeof(BanIP));
    if (!newList) {
        fprintf(stderr, "Failed to allocate IP ban list\n");
        exit(EXIT_FAILURE);
    }
    /* Zero‑initialize new slots */
    for (size_t i = bannedIPCapacity; i < newCap; ++i) {
        newList[i].ip = NULL;
        newList[i].time = 0;
        newList[i].reason = NULL;
    }
    bannedIPList = newList;
    bannedIPCapacity = newCap;
}

/* Helper: ensure capacity for HW list */
static void ensureHWCapacity(size_t needed) {
    if (bannedHWCapacity >= needed) return;
    size_t newCap = bannedHWCapacity ? bannedHWCapacity * 2 : 4;
    while (newCap < needed) newCap *= 2;
    BanHW *newList = realloc(bannedHWList, newCap * sizeof(BanHW));
    if (!newList) {
        fprintf(stderr, "Failed to allocate HW ban list\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = bannedHWCapacity; i < newCap; ++i) {
        newList[i].hw = NULL;
        newList[i].time = 0;
        newList[i].reason = NULL;
    }
    bannedHWList = newList;
    bannedHWCapacity = newCap;
}

/* Helper: free strings inside a BanIP */
static void freeBanIP(BanIP *b) {
    free(b->ip);
    free(b->reason);
    b->ip = NULL;
    b->reason = NULL;
}

/* Helper: free strings inside a BanHW */
static void freeBanHW(BanHW *b) {
    free(b->hw);
    free(b->reason);
    b->hw = NULL;
    b->reason = NULL;
}

/* ---------- IP bans ---------- */

void AddBannedIP(const char *ip, const char *reason, int duration) {
    if (CheckBannedIP(ip))
        return;

    /* Find first empty slot */
    size_t idx = (size_t)-1;
    for (size_t i = 0; i < bannedIPCount; ++i) {
        if (bannedIPList[i].ip == NULL || bannedIPList[i].ip[0] == '\0') {
            idx = i;
            break;
        }
    }

    if (idx == (size_t)-1) {
        /* Need new slot */
        ensureIPCapacity(bannedIPCount + 1);
        idx = bannedIPCount++;
    }

    freeBanIP(&bannedIPList[idx]);
    bannedIPList[idx].ip = strdup(ip);
    bannedIPList[idx].time = duration;
    bannedIPList[idx].reason = strdup(reason);

    free(lastBan);
    lastBan = strdup(ip);
}

bool DelBannedIP(const char *ip) {
    bool result = false;
    for (size_t i = 0; i < bannedIPCount; ++i) {
        if (bannedIPList[i].ip && strcmp(bannedIPList[i].ip, ip) == 0 && ip[0] != '\0') {
            freeBanIP(&bannedIPList[i]);
            /* Mark as empty */
            bannedIPList[i].ip = strdup("");
            bannedIPList[i].time = PERMANENT;
            bannedIPList[i].reason = strdup("");
            result = true;
        }
    }
    return result;
}

bool CheckBannedIP(const char *ip) {
    for (size_t i = 0; i < bannedIPCount; ++i) {
        if (bannedIPList[i].ip && bannedIPList[i].ip[0] != '\0') {
            if (fnmatch(bannedIPList[i].ip, ip, 0) == 0)
                return true;
        }
    }
    return false;
}

int FindBan(const char *ip) {
    for (size_t i = 0; i < bannedIPCount; ++i) {
        if (bannedIPList[i].ip && bannedIPList[i].ip[0] != '\0') {
            if (fnmatch(bannedIPList[i].ip, ip, 0) == 0)
                return (int)i;
        }
    }
    return -1;
}

void LoadBannedList(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;

    /* Reset list */
    for (size_t i = 0; i < bannedIPCount; ++i)
        freeBanIP(&bannedIPList[i]);
    bannedIPCount = 0;

    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        /* Strip trailing newline */
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') continue;

        /* Split by ':' into three parts */
        char *ip = strtok(line, ":");
        char *timeStr = strtok(NULL, ":");
        char *reason = strtok(NULL, ":");
        if (!ip || !timeStr) continue;

        long timeVal = strtol(timeStr, NULL, 10);
        if (timeVal == LONG_MIN) continue;   /* invalid */

        ensureIPCapacity(bannedIPCount + 1);
        bannedIPList[bannedIPCount].ip = strdup(ip);
        bannedIPList[bannedIPCount].time = (int)timeVal;
        bannedIPList[bannedIPCount].reason = reason ? strdup(reason) : strdup("");
        bannedIPCount++;
    }
    fclose(f);
}

void SaveBannedList(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return;

    for (size_t i = 0; i < bannedIPCount; ++i) {
        if (bannedIPList[i].ip && bannedIPList[i].ip[0] != '\0') {
            fprintf(f, "%s:%d:%s\r\n",
                    bannedIPList[i].ip,
                    bannedIPList[i].time,
                    bannedIPList[i].reason ? bannedIPList[i].reason : "");
        }
    }
    fclose(f);
}

void UpdateIPBanList(void) {
    for (size_t i = 0; i < bannedIPCount; ++i) {
        if (bannedIPList[i].ip && bannedIPList[i].ip[0] != '\0') {
            if (bannedIPList[i].time > 0 && bannedIPList[i].time != PERMANENT) {
                bannedIPList[i].time -= 3600;
                if (bannedIPList[i].time < 0)
                    bannedIPList[i].time = 0;
            }
            if (bannedIPList[i].time == 0 && bannedIPList[i].time != PERMANENT) {
                char msg[512];
                snprintf(msg, sizeof(msg),
                         "IP number %s (%s) unbanned",
                         bannedIPList[i].ip,
                         bannedIPList[i].reason ? bannedIPList[i].reason : "");
                MainConsole_Console(msg, CLIENT_MESSAGE_COLOR);
                DelBannedIP(bannedIPList[i].ip);
                SaveTxtLists();
            }
        }
    }
}

/* ---------- HW bans ---------- */

void AddBannedHW(const char *hw, const char *reason, int duration) {
    if (CheckBannedHW(hw))
        return;

    size_t idx = (size_t)-1;
    for (size_t i = 0; i < bannedHWCount; ++i) {
        if (bannedHWList[i].hw == NULL || bannedHWList[i].hw[0] == '\0') {
            idx = i;
            break;
        }
    }

    if (idx == (size_t)-1) {
        ensureHWCapacity(bannedHWCount + 1);
        idx = bannedHWCount++;
    }

    freeBanHW(&bannedHWList[idx]);
    bannedHWList[idx].hw = strdup(hw);
    bannedHWList[idx].time = duration;
    bannedHWList[idx].reason = strdup(reason);

    free(lastBanHW);
    lastBanHW = strdup(hw);
}

bool DelBannedHW(const char *hw) {
    bool result = false;
    for (size_t i = 0; i < bannedHWCount; ++i) {
        if (bannedHWList[i].hw && strcmp(bannedHWList[i].hw, hw) == 0 && hw[0] != '\0') {
            freeBanHW(&bannedHWList[i]);
            bannedHWList[i].hw = strdup("");
            bannedHWList[i].time = PERMANENT;
            bannedHWList[i].reason = strdup("");
            result = true;
        }
    }
    return result;
}

bool CheckBannedHW(const char *hw) {
    for (size_t i = 0; i < bannedHWCount; ++i) {
        if (bannedHWList[i].hw && bannedHWList[i].hw[0] != '\0') {
            if (fnmatch(bannedHWList[i].hw, hw, 0) == 0)
                return true;
        }
    }
    return false;
}

int FindBanHW(const char *hw) {
    for (size_t i = 0; i < bannedHWCount; ++i) {
        if (bannedHWList[i].hw && bannedHWList[i].hw[0] != '\0') {
            if (fnmatch(bannedHWList[i].hw, hw, 0) == 0)
                return (int)i;
        }
    }
    return -1;
}

void LoadBannedListHW(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;

    for (size_t i = 0; i < bannedHWCount; ++i)
        freeBanHW(&bannedHWList[i]);
    bannedHWCount = 0;

    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') continue;

        char *hw = strtok(line, ":");
        char *timeStr = strtok(NULL, ":");
        char *reason = strtok(NULL, ":");
        if (!hw || !timeStr) continue;

        long timeVal = strtol(timeStr, NULL, 10);
        if (timeVal == LONG_MIN) continue;

        ensureHWCapacity(bannedHWCount + 1);
        bannedHWList[bannedHWCount].hw = strdup(hw);
        bannedHWList[bannedHWCount].time = (int)timeVal;
        bannedHWList[bannedHWCount].reason = reason ? strdup(reason) : strdup("");
        bannedHWCount++;
    }
    fclose(f);
}

void SaveBannedListHW(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return;

    for (size_t i = 0; i < bannedHWCount; ++i) {
        if (bannedHWList[i].hw && bannedHWList[i].hw[0] != '\0') {
            fprintf(f, "%s:%d:%s\r\n",
                    bannedHWList[i].hw,
                    bannedHWList[i].time,
                    bannedHWList[i].reason ? bannedHWList[i].reason : "");
        }
    }
    fclose(f);
}

void UpdateHWBanList(void) {
    for (size_t i = 0; i < bannedHWCount; ++i) {
        if (bannedHWList[i].hw && bannedHWList[i].hw[0] != '\0') {
            if (bannedHWList[i].time > 0 && bannedHWList[i].time != PERMANENT) {
                bannedHWList[i].time -= 3600;
                if (bannedHWList[i].time < 0)
                    bannedHWList[i].time = 0;
            }
            if (bannedHWList[i].time == 0 && bannedHWList[i].time != PERMANENT) {
                char msg[512];
                snprintf(msg, sizeof(msg),
                         "Hardware ID %s (%s) unbanned",
                         bannedHWList[i].hw,
                         bannedHWList[i].reason ? bannedHWList[i].reason : "");
                MainConsole_Console(msg, CLIENT_MESSAGE_COLOR);
                DelBannedHW(bannedHWList[i].hw);
                SaveTxtLists();
            }
        }
    }
}

/* Cleanup (optional, call on program exit) */
void BanSystem_Free(void) {
    for (size_t i = 0; i < bannedIPCount; ++i) freeBanIP(&bannedIPList[i]);
    free(bannedIPList);
    bannedIPList = NULL;
    bannedIPCount = bannedIPCapacity = 0;

    for (size_t i = 0; i < bannedHWCount; ++i) freeBanHW(&bannedHWList[i]);
    free(bannedHWList);
    bannedHWList = NULL;
    bannedHWCount = bannedHWCapacity = 0;

    free(lastBan);
    free(lastBanHW);
}