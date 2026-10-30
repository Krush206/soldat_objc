#include "ScriptDateUtils.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* Constants */
#define DELPHI_EPOCH_OFFSET 25569.0   /* days between 1899-12-30 and 1970-01-01 */
#define SECONDS_PER_DAY 86400.0

/* Helper: convert TDateTime to time_t (UTC) */
static time_t DateTimeToTimeT(double dt)
{
    double days = dt - DELPHI_EPOCH_OFFSET;
    double seconds = days * SECONDS_PER_DAY;
    return (time_t)llround(seconds);
}

/* Helper: convert time_t to TDateTime (UTC) */
static double TimeTToDateTime(time_t t)
{
    return (double)t / SECONDS_PER_DAY + DELPHI_EPOCH_OFFSET;
}

/* ---------- Public API ---------- */

int64_t DateTimeToUnix(double d)
{
    return (int64_t)llround((d - DELPHI_EPOCH_OFFSET) * SECONDS_PER_DAY);
}

double UnixToDateTime(int64_t u)
{
    return (double)u / SECONDS_PER_DAY + DELPHI_EPOCH_OFFSET;
}

double StrToDateTime(const char *s)
{
    if (!s) return 0.0;
    struct tm tm = {0};
    /* Expected format: YYYY-MM-DD HH:MM:SS */
    if (strptime(s, "%Y-%m-%d %H:%M:%S", &tm) == NULL)
        return 0.0;
    time_t t = timegm(&tm);               /* UTC */
    if (t == (time_t)-1) return 0.0;
    return TimeTToDateTime(t);
}

double EncodeDate(uint16_t year, uint16_t month, uint16_t day)
{
    struct tm tm = {0};
    tm.tm_year = (int)year - 1900;
    tm.tm_mon  = (int)month - 1;
    tm.tm_mday = (int)day;
    tm.tm_hour = 0;
    tm.tm_min  = 0;
    tm.tm_sec  = 0;
    tm.tm_isdst = -1;
    time_t t = timegm(&tm);
    if (t == (time_t)-1) return 0.0;
    return TimeTToDateTime(t);
}

double EncodeTime(uint16_t hour, uint16_t min, uint16_t sec, uint16_t msec)
{
    if (hour > 23 || min > 59 || sec > 59 || msec > 999) return 0.0;
    double fraction = (hour * 3600.0 + min * 60.0 + sec + msec / 1000.0) / SECONDS_PER_DAY;
    return fraction;                     /* TDateTime with zero date part */
}

double EncodeDateTime(uint16_t year, uint16_t month, uint16_t day,
                      uint16_t hour, uint16_t min, uint16_t sec, uint16_t msec)
{
    double datePart = EncodeDate(year, month, day);
    double timePart = EncodeTime(hour, min, sec, msec);
    if (datePart == 0.0 || timePart == 0.0) return 0.0;
    return datePart + timePart;
}

bool TryEncodeDate(uint16_t year, uint16_t month, uint16_t day, double *outDate)
{
    double d = EncodeDate(year, month, day);
    if (d == 0.0) return false;
    *outDate = d;
    return true;
}

bool TryEncodeTime(uint16_t hour, uint16_t min, uint16_t sec, uint16_t msec, double *outTime)
{
    double t = EncodeTime(hour, min, sec, msec);
    if (t == 0.0 && !(hour == 0 && min == 0 && sec == 0 && msec == 0)) return false;
    *outTime = t;
    return true;
}

bool TryEncodeDateTime(uint16_t year, uint16_t month, uint16_t day,
                       uint16_t hour, uint16_t min, uint16_t sec, uint16_t msec,
                       double *outDateTime)
{
    double dt = EncodeDateTime(year, month, day, hour, min, sec, msec);
    if (dt == 0.0) return false;
    *outDateTime = dt;
    return true;
}

/* Convert TDateTime to broken-down UTC time */
static void DateTimeToTm(double dt, struct tm *tm)
{
    time_t t = DateTimeToTimeT(dt);
    gmtime_r(&t, tm);
}

void DecodeDate(double dateTime, uint16_t *year, uint16_t *month, uint16_t *day)
{
    struct tm tm;
    DateTimeToTm(dateTime, &tm);
    if (year)  *year  = (uint16_t)(tm.tm_year + 1900);
    if (month) *month = (uint16_t)(tm.tm_mon + 1);
    if (day)   *day   = (uint16_t)tm.tm_mday;
}

void DecodeTime(double dateTime, uint16_t *hour, uint16_t *min, uint16_t *sec, uint16_t *msec)
{
    struct tm tm;
    DateTimeToTm(dateTime, &tm);
    if (hour) *hour = (uint16_t)tm.tm_hour;
    if (min)  *min  = (uint16_t)tm.tm_min;
    if (sec)  *sec  = (uint16_t)tm.tm_sec;
    if (msec) *msec = 0;                     /* TDateTime has no millisecond precision */
}

void DecodeDateTime(double dateTime,
                    uint16_t *year, uint16_t *month, uint16_t *day,
                    uint16_t *hour, uint16_t *min, uint16_t *sec, uint16_t *msec)
{
    DecodeDate(dateTime, year, month, day);
    DecodeTime(dateTime, hour, min, sec, msec);
}

/* Delphi DayOfWeek: 1=Monday ... 7=Sunday */
uint16_t DayOfWeek(double dateTime)
{
    struct tm tm;
    DateTimeToTm(dateTime, &tm);
    int w = tm.tm_wday;               /* 0=Sun .. 6=Sat */
    return (uint16_t)((w + 6) % 7 + 1);   /* map to 1=Mon */
}

double Date(void)
{
    double now = Now();
    /* Strip time fraction */
    double intPart;
    modf(now, &intPart);
    return intPart;
}

double Time(void)
{
    double now = Now();
    double intPart;
    double frac = modf(now, &intPart);
    if (frac < 0) frac += 1.0;   /* just in case */
    return frac;
}

double Now(void)
{
    time_t t = time(NULL);
    return TimeTToDateTime(t);
}

char *DateToStr(double dateTime)
{
    struct tm tm;
    DateTimeToTm(dateTime, &tm);
    char *buf = malloc(11);               /* YYYY-MM-DD\0 */
    if (!buf) return NULL;
    strftime(buf, 11, "%Y-%m-%d", &tm);
    return buf;
}

double StrToDate(const char *s)
{
    if (!s) return 0.0;
    struct tm tm = {0};
    if (strptime(s, "%Y-%m-%d", &tm) == NULL) return 0.0;
    time_t t = timegm(&tm);
    if (t == (time_t)-1) return 0.0;
    return TimeTToDateTime(t);
}

/* Very small subset of Delphi format tokens */
char *FormatDateTime(const char *fmt, double dateTime)
{
    if (!fmt) return NULL;
    struct tm tm;
    DateTimeToTm(dateTime, &tm);

    /* First pass: compute needed size */
    size_t needed = 1; /* null terminator */
    for (const char *p = fmt; *p; ++p) {
        if (*p == '\'') { /* skip quoted literal */
            ++p;
            while (*p && *p != '\'') { ++needed; ++p; }
            continue;
        }
        if (*p == 'y' || *p == 'Y') { /* year tokens */
            /* count consecutive y */
            int cnt = 0;
            while (p[cnt] && (p[cnt]=='y'||p[cnt]=='Y')) ++cnt;
            needed += (cnt >= 4) ? 4 : 2;
            p += cnt - 1;
            continue;
        }
        if (*p == 'm' || *p == 'M') {
            int cnt = 0;
            while (p[cnt] && (p[cnt]=='m'||p[cnt]=='M')) ++cnt;
            needed += 2;                 /* month always two digits */
            p += cnt - 1;
            continue;
        }
        if (*p == 'd' || *p == 'D') {
            int cnt = 0;
            while (p[cnt] && (p[cnt]=='d'||p[cnt]=='D')) ++cnt;
            needed += 2;                 /* day two digits */
            p += cnt - 1;
            continue;
        }
        if (*p == 'h' || *p == 'H') {
            int cnt = 0;
            while (p[cnt] && (p[cnt]=='h'||p[cnt]=='H')) ++cnt;
            needed += 2;
            p += cnt - 1;
            continue;
        }
        if (*p == 'n' || *p == 'N') {
            int cnt = 0;
            while (p[cnt] && (p[cnt]=='n'||p[cnt]=='N')) ++cnt;
            needed += 2;
            p += cnt - 1;
            continue;
        }
        if (*p == 's' || *p == 'S') {
            int cnt = 0;
            while (p[cnt] && (p[cnt]=='s'||p[cnt]=='S')) ++cnt;
            needed += 2;
            p += cnt - 1;
            continue;
        }
        if (*p == 'z' || *p == 'Z') {
            int cnt = 0;
            while (p[cnt] && (p[cnt]=='z'||p[cnt]=='Z')) ++cnt;
            needed += 3;                 /* milliseconds three digits */
            p += cnt - 1;
            continue;
        }
        /* any other character copied literally */
        ++needed;
    }

    char *out = malloc(needed);
    if (!out) return NULL;
    char *dst = out;

    for (const char *p = fmt; *p; ++p) {
        if (*p == '\'') {
            ++p;
            while (*p && *p != '\'') {
                *dst++ = *p++;
            }
            continue;
        }
        if (*p == 'y' || *p == 'Y') {
            int cnt = 0;
            while (p[cnt] && (p[cnt]=='y'||p[cnt]=='Y')) ++cnt;
            if (cnt >= 4) {
                snprintf(dst, 5, "%04d", tm.tm_year + 1900);
                dst += 4;
            } else {
                snprintf(dst, 3, "%02d", (tm.tm_year + 1900) % 100);
                dst += 2;
            }
            p += cnt - 1;
            continue;
        }
        if (*p == 'm' || *p == 'M') {
            int cnt = 0;
            while (p[cnt] && (p[cnt]=='m'||p[cnt]=='M')) ++cnt;
            snprintf(dst, 3, "%02d", tm.tm_mon + 1);
            dst += 2;
            p += cnt - 1;
            continue;
        }
        if (*p == 'd' || *p == 'D') {
            int cnt = 0;
            while (p[cnt] && (p[cnt]=='d'||p[cnt]=='D')) ++cnt;
            snprintf(dst, 3, "%02d", tm.tm_mday);
            dst += 2;
            p += cnt - 1;
            continue;
        }
        if (*p == 'h' || *p == 'H') {
            int cnt = 0;
            while (p[cnt] && (p[cnt]=='h'||p[cnt]=='H')) ++cnt;
            snprintf(dst, 3, "%02d", tm.tm_hour);
            dst += 2;
            p += cnt - 1;
            continue;
        }
        if (*p == 'n' || *p == 'N') {
            int cnt = 0;
            while (p[cnt] && (p[cnt]=='n'||p[cnt]=='N')) ++cnt;
            snprintf(dst, 3, "%02d", tm.tm_min);
            dst += 2;
            p += cnt - 1;
            continue;
        }
        if (*p == 's' || *p == 'S') {
            int cnt = 0;
            while (p[cnt] && (p[cnt]=='s'||p[cnt]=='S')) ++cnt;
            snprintf(dst, 3, "%02d", tm.tm_sec);
            dst += 2;
            p += cnt - 1;
            continue;
        }
        if (*p == 'z' || *p == 'Z') {
            int cnt = 0;
            while (p[cnt] && (p[cnt]=='z'||p[cnt]=='Z')) ++cnt;
            snprintf(dst, 4, "%03d", 0);   /* no ms precision */
            dst += 3;
            p += cnt - 1;
            continue;
        }
        *dst++ = *p;
    }
    *dst = '\0';
    return out;
}