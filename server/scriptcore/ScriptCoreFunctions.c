#include "ScriptCoreFunctions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <regex.h>
#include <openssl/md5.h>

/* ------------------------------------------------------------------ */
/* Helper: duplicate a string (caller must free)                       */
/* ------------------------------------------------------------------ */
static char *strdup_safe(const char *s)
{
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);
    if (copy) memcpy(copy, s, len);
    return copy;
}

/* ------------------------------------------------------------------ */
/* WriteLn – prints a line to the server console (stdout here)        */
/* ------------------------------------------------------------------ */
void WriteLn(const char *Text)
{
    if (Text)
        printf("%s\n", Text);
    else
        printf("\n");
}

/* ------------------------------------------------------------------ */
/* StrToInt – convert string to integer (base 10)                     */
/* ------------------------------------------------------------------ */
int StrToInt(const char *Text)
{
    if (!Text) return 0;
    return atoi(Text);
}

/* ------------------------------------------------------------------ */
/* iif – inline if, returns IfTrue when Condition != 0 else IfFalse   */
/* ------------------------------------------------------------------ */
void *iif(int Condition, void *IfTrue, void *IfFalse)
{
    return Condition ? IfTrue : IfFalse;
}

/* ------------------------------------------------------------------ */
/* MD5 – returns a newly allocated 32‑character hex string            */
/* ------------------------------------------------------------------ */
char *MD5(const char *A)
{
    if (!A) return strdup_safe("");
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5((const unsigned char *)A, strlen(A), digest);

    char *out = malloc(33);
    if (!out) return NULL;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
        sprintf(out + i * 2, "%02x", digest[i]);
    out[32] = '\0';
    return out;
}

/* ------------------------------------------------------------------ */
/* Math wrappers                                                      */
/* ------------------------------------------------------------------ */
double ArcTan(double A) { return atan(A); }
double ExpFunc(double A) { return exp(A); }
double Ln(double A) { return log(A); }
double Log10(double A) { return log10(A); }
double LogN(double A, double B) { return log(A) / log(B); }

/* ------------------------------------------------------------------ */
/* RandomInt – inclusive range [Min, Max]                             */
/* ------------------------------------------------------------------ */
int RandomInt(int Min, int Max)
{
    if (Min > Max) { int t = Min; Min = Max; Max = t; }
    /* rand() returns 0..RAND_MAX */
    return Min + (int)((double)rand() / (RAND_MAX + 1.0) * (Max - Min + 1));
}

/* ------------------------------------------------------------------ */
/* RGB – pack three bytes into a 24‑bit colour value (0xRRGGBB)        */
/* ------------------------------------------------------------------ */
LongWord RGB(Byte R, Byte G, Byte B)
{
    return ((LongWord)B) | ((LongWord)G << 8) | ((LongWord)R << 16);
}

/* ------------------------------------------------------------------ */
/* Distance – Euclidean distance between two points (X1,Y1) (X2,Y2)   */
/* ------------------------------------------------------------------ */
float Distance(float X1, float X2, float Y1, float Y2)
{
    float dx = X1 - X2;
    float dy = Y1 - Y2;
    return sqrtf(dx * dx + dy * dy);
}

/* ------------------------------------------------------------------ */
/* Regular expression helpers (POSIX regex)                           */
/* ------------------------------------------------------------------ */
int ExecRegExpr(const char *ARegExpr, const char *AInputStr)
{
    if (!ARegExpr || !AInputStr) return 0;
    regex_t regex;
    int ret = regcomp(&regex, ARegExpr, REG_EXTENDED | REG_NOSUB);
    if (ret) return 0;
    ret = regexec(&regex, AInputStr, 0, NULL, 0);
    regfree(&regex);
    return ret == 0;
}

/* SplitRegExpr – fills APieces with substrings matching the regex.
   Returns number of pieces (>=0). Caller must free each string and the array. */
int SplitRegExpr(const char *ARegExpr, const char *AInputStr,
                 char ***APieces, int *count)
{
    if (!ARegExpr || !AInputStr || !APieces || !count) return -1;
    regex_t regex;
    if (regcomp(&regex, ARegExpr, REG_EXTENDED) != 0) return -1;

    regmatch_t matches[1];
    const char *p = AInputStr;
    int capacity = 4, used = 0;
    char **pieces = malloc(capacity * sizeof(char *));
    if (!pieces) { regfree(&regex); return -1; }

    while (regexec(&regex, p, 1, matches, 0) == 0) {
        if (matches[0].rm_so == matches[0].rm_eo) break; /* avoid infinite loop */
        size_t len = matches[0].rm_eo - matches[0].rm_so;
        char *piece = malloc(len + 1);
        if (!piece) break;
        memcpy(piece, p + matches[0].rm_so, len);
        piece[len] = '\0';
        if (used >= capacity) {
            capacity *= 2;
            char **tmp = realloc(pieces, capacity * sizeof(char *));
            if (!tmp) break;
            pieces = tmp;
        }
        pieces[used++] = piece;
        p += matches[0].rm_eo;
    }
    regfree(&regex);
    *APieces = pieces;
    *count = used;
    return used;
}

/* ReplaceRegExpr – simple global replace using POSIX regex.
   AUseSubstitution: if non‑zero, back‑references (\1, \2 …) are expanded. */
char *ReplaceRegExpr(const char *ARegExpr, const char *AInputStr,
                     const char *AReplaceStr, int AUseSubstitution)
{
    if (!ARegExpr || !AInputStr || !AReplaceStr) return strdup_safe("");
    regex_t regex;
    if (regcomp(&regex, ARegExpr, REG_EXTENDED) != 0) return strdup_safe("");

    /* Estimate output size */
    size_t out_cap = strlen(AInputStr) * 2 + 1;
    char *output = malloc(out_cap);
    if (!output) { regfree(&regex); return strdup_safe(""); }
    output[0] = '\0';

    const char *src = AInputStr;
    regmatch_t matches[10]; /* up to 9 sub‑expressions */
    while (regexec(&regex, src, 10, matches, 0) == 0) {
        /* copy text before match */
        size_t pre_len = matches[0].rm_so;
        if (strlen(output) + pre_len + 1 >= out_cap) {
            out_cap *= 2;
            output = realloc(output, out_cap);
        }
        strncat(output, src, pre_len);

        /* build replacement */
        const char *rep = AReplaceStr;
        while (*rep) {
            if (AUseSubstitution && *rep == '\\' && rep[1] >= '0' && rep[1] <= '9') {
                int idx = rep[1] - '0';
                rep += 2;
                if (idx < 10 && matches[idx].rm_so >= 0) {
                    size_t slen = matches[idx].rm_eo - matches[idx].rm_so;
                    if (strlen(output) + slen + 1 >= out_cap) {
                        out_cap *= 2;
                        output = realloc(output, out_cap);
                    }
                    strncat(output, src + matches[idx].rm_so, slen);
                }
            } else {
                size_t olen = strlen(output);
                if (olen + 2 >= out_cap) {
                    out_cap *= 2;
                    output = realloc(output, out_cap);
                }
                output[olen] = *rep++;
                output[olen + 1] = '\0';
            }
        }
        src += matches[0].rm_eo;
    }
    /* copy remainder */
    strcat(output, src);
    regfree(&regex);
    return output;
}

/* QuoteRegExprMetaChars – escape characters that have special meaning in POSIX ERE */
char *QuoteRegExprMetaChars(const char *AStr)
{
    if (!AStr) return strdup_safe("");
    const char *meta = ".^$*+?(){}[\\]|";
    size_t len = strlen(AStr);
    char *out = malloc(len * 2 + 1);
    if (!out) return NULL;
    char *dst = out;
    for (size_t i = 0; i < len; ++i) {
        if (strchr(meta, AStr[i])) *dst++ = '\\';
        *dst++ = AStr[i];
    }
    *dst = '\0';
    return out;
}

/* RegExprSubExpressions – returns number of capturing groups and fills ASubExprs
   with the text of each sub‑expression for the first match. */
int RegExprSubExpressions(const char *ARegExpr,
                          char ***ASubExprs, int *count,
                          int AExtendedSyntax)
{
    if (!ARegExpr || !ASubExprs || !count) return -1;
    regex_t regex;
    int cflags = REG_EXTENDED;
    if (!AExtendedSyntax) cflags = 0; /* basic regex */
    if (regcomp(&regex, ARegExpr, cflags) != 0) return -1;

    int nsub = regex.re_nsub;
    char **subs = calloc(nsub, sizeof(char *));
    if (!subs) { regfree(&regex); return -1; }

    /* We need a subject string – the API in Pascal expects the caller to
       provide it elsewhere; here we just initialise empty strings. */
    for (int i = 0; i < nsub; ++i)
        subs[i] = strdup_safe("");

    *ASubExprs = subs;
    *count = nsub;
    regfree(&regex);
    return nsub;
}

/* FormatFloat – uses snprintf with the supplied format string */
char *FormatFloat(const char *Format, double Value)
{
    if (!Format) return strdup_safe("");
    char buffer[256];
    snprintf(buffer, sizeof(buffer), Format, Value);
    return strdup_safe(buffer);
}

/* ------------------------------------------------------------------ */
/* Weapon / object / menu conversion tables                           */
/* ------------------------------------------------------------------ */
Byte weap2obj(Byte Style)
{
    switch (Style) {
        case EAGLE_NUM:       return OBJECT_DESERT_EAGLE;
        case MP5_NUM:         return OBJECT_HK_MP5;
        case AK74_NUM:        return OBJECT_AK74;
        case STEYRAUG_NUM:    return OBJECT_STEYR_AUG;
        case SPAS12_NUM:      return OBJECT_SPAS12;
        case RUGER77_NUM:     return OBJECT_RUGER77;
        case M79_NUM:         return OBJECT_M79;
        case BARRETT_NUM:     return OBJECT_BARRET_M82A1;
        case M249_NUM:        return OBJECT_MINIMI;
        case MINIGUN_NUM:     return OBJECT_MINIGUN;
        case COLT_NUM:        return OBJECT_USSOCOM;
        case KNIFE_NUM:       return OBJECT_COMBAT_KNIFE;
        case CHAINSAW_NUM:    return OBJECT_CHAINSAW;
        case LAW_NUM:         return OBJECT_LAW;
        default:
            fprintf(stderr, "Invalid weapon style: %d\n", Style);
            return 0;
    }
}

Byte menu2obj(Byte Style)
{
    switch (Style) {
        case EAGLE:        return OBJECT_DESERT_EAGLE;
        case MP5:          return OBJECT_HK_MP5;
        case AK74:         return OBJECT_AK74;
        case STEYRAUG:     return OBJECT_STEYR_AUG;
        case SPAS12:       return OBJECT_SPAS12;
        case RUGER77:      return OBJECT_RUGER77;
        case M79:          return OBJECT_M79;
        case BARRETT:      return OBJECT_BARRET_M82A1;
        case M249:         return OBJECT_MINIMI;
        case MINIGUN:      return OBJECT_MINIGUN;
        case COLT:         return OBJECT_USSOCOM;
        case KNIFE:        return OBJECT_COMBAT_KNIFE;
        case CHAINSAW:     return OBJECT_CHAINSAW;
        case LAW:          return OBJECT_LAW;
        default:
            fprintf(stderr, "Invalid weapon menu style: %d\n", Style);
            return 0;
    }
}

Byte obj2weap(Byte Style)
{
    switch (Style) {
        case OBJECT_DESERT_EAGLE:   return EAGLE_NUM;
        case OBJECT_HK_MP5:         return MP5_NUM;
        case OBJECT_AK74:           return AK74_NUM;
        case OBJECT_STEYR_AUG:      return STEYRAUG_NUM;
        case OBJECT_SPAS12:         return SPAS12_NUM;
        case OBJECT_RUGER77:        return RUGER77_NUM;
        case OBJECT_M79:            return M79_NUM;
        case OBJECT_BARRET_M82A1:   return BARRETT_NUM;
        case OBJECT_MINIMI:         return M249_NUM;
        case OBJECT_MINIGUN:        return MINIGUN_NUM;
        case OBJECT_USSOCOM:        return COLT_NUM;
        case OBJECT_COMBAT_KNIFE:   return KNIFE_NUM;
        case OBJECT_CHAINSAW:       return CHAINSAW_NUM;
        case OBJECT_LAW:            return LAW_NUM;
        default:
            fprintf(stderr, "Invalid object style: %d\n", Style);
            return 0;
    }
}

Byte obj2menu(Byte Style)
{
    switch (Style) {
        case OBJECT_DESERT_EAGLE:   return EAGLE;
        case OBJECT_HK_MP5:         return MP5;
        case OBJECT_AK74:           return AK74;
        case OBJECT_STEYR_AUG:      return STEYRAUG;
        case OBJECT_SPAS12:         return SPAS12;
        case OBJECT_RUGER77:        return RUGER77;
        case OBJECT_M79:            return M79;
        case OBJECT_BARRET_M82A1:   return BARRETT;
        case OBJECT_MINIMI:         return M249;
        case OBJECT_MINIGUN:        return MINIGUN;
        case OBJECT_USSOCOM:        return COLT;
        case OBJECT_COMBAT_KNIFE:   return KNIFE;
        case OBJECT_CHAINSAW:       return CHAINSAW;
        case OBJECT_LAW:            return LAW;
        default:
            fprintf(stderr, "Invalid object style: %d\n", Style);
            return 0;
    }
}

Byte weap2menu(Byte Style)
{
    switch (Style) {
        case EAGLE_NUM:    return EAGLE;
        case MP5_NUM:      return MP5;
        case AK74_NUM:     return AK74;
        case STEYRAUG_NUM: return STEYRAUG;
        case SPAS12_NUM:   return SPAS12;
        case RUGER77_NUM:  return RUGER77;
        case M79_NUM:      return M79;
        case BARRETT_NUM:  return BARRETT;
        case M249_NUM:     return M249;
        case MINIGUN_NUM:  return MINIGUN;
        case COLT_NUM:     return COLT;
        case KNIFE_NUM:    return KNIFE;
        case CHAINSAW_NUM: return CHAINSAW;
        case LAW_NUM:      return LAW;
        case NOWEAPON_NUM: return NOWEAPON;
        default:
            fprintf(stderr, "Invalid weapon style: %d\n", Style);
            return 0;
    }
}

Byte menu2weap(Byte Style)
{
    switch (Style) {
        case EAGLE:    return EAGLE_NUM;
        case MP5:      return MP5_NUM;
        case AK74:     return AK74_NUM;
        case STEYRAUG: return STEYRAUG_NUM;
        case SPAS12:   return SPAS12_NUM;
        case RUGER77:  return RUGER77_NUM;
        case M79:      return M79_NUM;
        case BARRETT:  return BARRETT_NUM;
        case M249:     return M249_NUM;
        case MINIGUN:  return MINIGUN_NUM;
        case COLT:     return COLT_NUM;
        case KNIFE:    return KNIFE_NUM;
        case CHAINSAW: return CHAINSAW_NUM;
        case LAW:      return LAW_NUM;
        default:
            fprintf(stderr, "Invalid weapon style: %d\n", Style);
            return 0;
    }
}