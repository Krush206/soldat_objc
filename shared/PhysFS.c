#include "PhysFS.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <limits.h>

/* Reads the whole file into a malloc'd buffer; size returned via *outSize.
 * Returns NULL when missing. */
unsigned char *PHYSFS_readBuffer(const char *name, int *outSize)
{
    if (!PHYSFS_exists(name)) {
        return NULL;
    }

    PHYSFS_File *file = PHYSFS_openRead(name);
    if (!file) {
        return NULL;
    }

    int64_t len = PHYSFS_fileLength(file);
    if (len < 0 || len > INT_MAX) {
        PHYSFS_close(file);
        return NULL;
    }

    unsigned char *buf = (unsigned char *)malloc((size_t)len);
    if (!buf) {
        PHYSFS_close(file);
        return NULL;
    }

    int64_t read = PHYSFS_readBytes(file, buf, (uint64_t)len);
    PHYSFS_close(file);

    if (read != len) {
        free(buf);
        return NULL;
    }

    *outSize = (int)len;
    return buf;
}

/* Reads a single line (up to and excluding the newline) into buf. */
void PHYSFS_ReadLn(PHYSFS_File *handle, char *buf, int bufsize)
{
    int pos = 0;
    char c;

    if (bufsize <= 0) {
        return;
    }

    while (pos < bufsize - 1) {
        int64_t r = PHYSFS_readBytes(handle, &c, 1);
        if (r != 1) {
            break;
        }
        if (c == '\n') {
            break;
        }
        if (c != '\r') {
            buf[pos++] = c;
        }
    }
    buf[pos] = '\0';
}

int PHYSFS_CopyFileFromArchive(const char *sourceFile, const char *destination)
{
    struct stat st;
    if (stat(destination, &st) == 0) {
        return 0; /* Already exists */
    }

    int size;
    unsigned char *data = PHYSFS_readBuffer(sourceFile, &size);
    if (!data || size <= 0) {
        free(data);
        return 0;
    }

    FILE *fp = fopen(destination, "wb");
    if (!fp) {
        free(data);
        return 0;
    }

    size_t written = fwrite(data, 1, (size_t)size, fp);
    fclose(fp);
    free(data);

    return written == (size_t)size ? 1 : 0;
}

/* Enumerates dir; returns a malloc'd array of strings, count via *outCount. */
char **PHYSFS_GetEnumeratedFiles(const char *dir, int *outCount)
{
    char **list = PHYSFS_enumerateFiles(dir);
    if (!list) {
        *outCount = 0;
        return NULL;
    }

    /* Count entries */
    int count = 0;
    while (list[count]) {
        count++;
    }

    char **result = (char **)malloc((count + 1) * sizeof(char *));
    if (!result) {
        PHYSFS_freeList(list);
        *outCount = 0;
        return NULL;
    }

    for (int i = 0; i < count; i++) {
        result[i] = strdup(list[i]);
        if (!result[i]) {
            /* Cleanup on failure */
            for (int j = 0; j < i; j++) {
                free(result[j]);
            }
            free(result);
            PHYSFS_freeList(list);
            *outCount = 0;
            return NULL;
        }
    }
    result[count] = NULL;

    PHYSFS_freeList(list);
    *outCount = count;
    return result;
}