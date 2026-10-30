/* Util — C port of shared/Util.pas */

#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdbool.h>
#include "Sha1.h"

typedef struct TMapInfo {
    char *Name;
    char *MapName;
    uint64_t WorkshopID;
    char *Path;
} TMapInfo;

/* Gets a specific piece of a string */
char *get_piece(const char *source, const char *delimiter, int piece);
unsigned int color_to_hex(unsigned int color);
unsigned int string_to_color(const char *s);
char *number_format(unsigned int num);
int check_file_size(const char *filename);
char *override_file_ext(const char *filename, const char *ext);
char *md5_string_helper(const char *text);
bool create_dir_if_missing(const char *dir);
bool create_file_if_missing(const char *filename);
char *get_size(int64_t bytes);
void get_map_checksum(TMapInfo map, TSHA1Digest *digest);
bool get_map_info(const char *mapname, const char *directory, TMapInfo *mapinfo);
bool verify_map_checksum(TMapInfo map, TSHA1Digest checksum);

#endif /* UTIL_H */