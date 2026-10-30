#ifndef SCRIPT_DATE_UTILS_H
#define SCRIPT_DATE_UTILS_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* Convert a Delphi TDateTime (double, days since 30 Dec 1899) to Unix timestamp (seconds since 1 Jan 1970). */
int64_t DateTimeToUnix(double d);

/* Convert Unix timestamp to Delphi TDateTime. */
double UnixToDateTime(int64_t u);

/* Parse a date/time string (format: "YYYY-MM-DD HH:MM:SS") into TDateTime.
   Returns 0.0 on failure. */
double StrToDateTime(const char *s);

/* Encode a date (year, month, day) into TDateTime. Returns 0.0 on invalid. */
double EncodeDate(uint16_t year, uint16_t month, uint16_t day);

/* Encode a time (hour, min, sec, msec) into TDateTime (fraction of day). Returns 0.0 on invalid. */
double EncodeTime(uint16_t hour, uint16_t min, uint16_t sec, uint16_t msec);

/* Encode date and time together. Returns 0.0 on invalid. */
double EncodeDateTime(uint16_t year, uint16_t month, uint16_t day,
                      uint16_t hour, uint16_t min, uint16_t sec, uint16_t msec);

/* TryEncode variants return bool and set output parameter. */
bool TryEncodeDate(uint16_t year, uint16_t month, uint16_t day, double *outDate);
bool TryEncodeTime(uint16_t hour, uint16_t min, uint16_t sec, uint16_t msec, double *outTime);
bool TryEncodeDateTime(uint16_t year, uint16_t month, uint16_t day,
                       uint16_t hour, uint16_t min, uint16_t sec, uint16_t msec,
                       double *outDateTime);

/* Decode functions fill the provided pointers. */
void DecodeDate(double dateTime, uint16_t *year, uint16_t *month, uint16_t *day);
void DecodeTime(double dateTime, uint16_t *hour, uint16_t *min, uint16_t *sec, uint16_t *msec);
void DecodeDateTime(double dateTime,
                    uint16_t *year, uint16_t *month, uint16_t *day,
                    uint16_t *hour, uint16_t *min, uint16_t *sec, uint16_t *msec);

/* Day of week: 1 = Monday ... 7 = Sunday (matches Delphi). */
uint16_t DayOfWeek(double dateTime);

/* Current date (midnight), current time (fraction), and now (date+time). */
double Date(void);
double Time(void);
double Now(void);

/* Convert TDateTime to string using default locale format (YYYY-MM-DD). */
char *DateToStr(double dateTime);   /* caller must free */

/* Parse date string (YYYY-MM-DD) into TDateTime. Returns 0.0 on failure. */
double StrToDate(const char *s);

/* Format TDateTime according to a format string (subset of Delphi formats).
   Supported tokens: yyyy, yy, mm, dd, hh, nn, ss, zzz.
   Returns newly allocated string; caller must free. */
char *FormatDateTime(const char *fmt, double dateTime);

#endif /* SCRIPT_DATE_UTILS_H */