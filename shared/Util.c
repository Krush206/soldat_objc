/* Util — C port of shared/Util.pas */
#include "Util.h"
#include "SysUtils.h"
#include "Sha1.h"
#include "Md5.h"
#include "PhysFS.h"
#include "Constants.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

/* Gets a specific piece of a string */
char *get_piece(const char *source, const char *delimiter, int piece) {
    if (!source || !delimiter || piece < 1) {
        return strdup2("");
    }

    char *src_copy = strdup2(source);
    if (!src_copy) return strdup2("");

    char *result = strdup2("");
    char *token = strtok(src_copy, delimiter);
    int current = 1;

    while (token && current <= piece) {
        if (current == piece) {
            free(result);
            result = strdup2(token);
            break;
        }
        token = strtok(NULL, delimiter);
        current++;
    }

    free(src_copy);
    return result;
}

unsigned int color_to_hex(unsigned int color) {
    while ((color & 0xFF000000) > 0) {
        color = color >> 4;
    }

    unsigned int result =
        0xFF000000 |
        ((color & 0xFF) << 16) |
        (color & 0xFF00) |
        ((color & 0xFF0000) >> 16);

    return result;
}

unsigned int string_to_color(const char *s) {
    if (!s) return 0xFFFFFFFF;
    
    unsigned int color = 0;
    size_t len = strlen(s);
    
    if (len >= 1 && s[0] == '$') s++;
    else if (len >= 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) s += 2;

    for (const char *p = s; *p; p++) {
        char c = *p;
        int val = 0;
        if (c >= '0' && c <= '9') val = c - '0';
        else if (c >= 'A' && c <= 'F') val = c - 'A' + 10;
        else if (c >= 'a' && c <= 'f') val = c - 'a' + 10;
        else continue;
        color = (color << 4) | val;
    }
    
    return color;
}

char *number_format(unsigned int num) {
    char *buf = malloc(32);
    if (!buf) return NULL;
    
    int len = snprintf(buf, 32, "%u", num);
    if (len <= 0) { free(buf); return strdup2("0"); }
    
    /* Add thousands separators */
    int comma_count = (len - 1) / 3;
    char *result = malloc(len + comma_count + 1);
    if (!result) { free(buf); return NULL; }
    
    int pos = 0;
    for (int i = 0; i < len; i++) {
        if (i > 0 && (len - i) % 3 == 0) {
            result[pos++] = ',';
        }
        result[pos++] = buf[i];
    }
    result[pos] = '\0';
    
    free(buf);
    return result;
}

int check_file_size(const char *filename) {
    if (!filename) return -1;
    
    struct stat st;
    if (stat(filename, &st) == 0) {
        return (int)st.st_size;
    }
    return -1;
}

char *override_file_ext(const char *filename, const char *ext) {
    if (!filename) return strdup2("");
    if (!ext) ext = "";
    
    char *result = strdup2(filename);
    if (!result) return NULL;
    
    char *dot = strrchr(result, '.');
    char *slash = strrchr(result, '/');
    
    if (dot && (!slash || dot > slash)) {
        *dot = '\0';
    }
    
    size_t new_len = strlen(result) + strlen(ext) + 2;
    char *new_result = realloc(result, new_len);
    if (!new_result) { free(result); return NULL; }
    result = new_result;
    
    strcat(result, ".");
    strcat(result, ext);
    
    return result;
}

char *md5_string_helper(const char *text) {
    if (!text) return strdup2("");
    TMD5Digest digest;
    MD5String(text, &digest);
    return MD5Print(&digest);
}

bool create_dir_if_missing(const char *dir) {
    if (!dir) return false;
    
    struct stat st;
    if (stat(dir, &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    
    /* Create directory recursively */
    char *path = strdup2(dir);
    if (!path) return false;
    
    for (char *p = path + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(path, 0755) != 0) {
                /* Ignore EEXIST */
            }
            *p = '/';
        }
    }
    int ret = mkdir(path, 0755);
    free(path);
    return ret == 0;
}

bool create_file_if_missing(const char *filename) {
    if (!filename) return false;
    
    FILE *f = fopen(filename, "a");
    if (f) {
        fclose(f);
        return true;
    }
    return false;
}

char *get_size(int64_t bytes) {
    const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;
    double size = (double)bytes;
    
    while (size >= 1024.0 && unit < 4) {
        size /= 1024.0;
        unit++;
    }
    
    char *buf = malloc(32);
    if (!buf) return NULL;
    
    if (unit == 0) {
        snprintf(buf, 32, "%d %s", (int)size, units[unit]);
    } else {
        snprintf(buf, 32, "%.1f %s", size, units[unit]);
    }
    
    return buf;
}

void get_map_checksum(TMapInfo map, TSHA1Digest *digest) {
    if (!digest) return;
    memset(digest, 0, sizeof(TSHA1Digest));
    /* TODO: Implement SHA1 checksum for map */
}

bool get_map_info(const char *mapname, const char *directory, TMapInfo *mapinfo) {
    if (!mapname || !directory || !mapinfo) return false;
    
    memset(mapinfo, 0, sizeof(TMapInfo));
    mapinfo->Name = strdup2(mapname);
    mapinfo->MapName = strdup2(mapname);
    mapinfo->WorkshopID = 0;
    
    size_t path_len = strlen(directory) + strlen(mapname) + 2;
    mapinfo->Path = malloc(path_len);
    if (!mapinfo->Path) return false;
    snprintf(mapinfo->Path, path_len, "%s/%s", directory, mapname);
    
    return true;
}

bool verify_map_checksum(TMapInfo map, TSHA1Digest checksum) {
    (void)map;
    (void)checksum;
    return true;
}