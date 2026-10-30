/* PhysFS — Objective-C port of the FreePascal PhysFS binding used by
 * OpenSoldat. Declares the subset of the physfs C API the game uses plus
 * the helper functions from shared/libs/PhysFS/PhysFS.pas. */

#ifndef PHYSFS_PORT_H
#define PHYSFS_PORT_H

#include <stdint.h>

typedef void PHYSFS_File;

/* Raw physfs C API (libphysfs). */
#ifdef __cplusplus
extern "C" {
#endif

int PHYSFS_init(const char *argv0);
int PHYSFS_deinit(void);
int PHYSFS_mount(const char *newDir, const char *mountPoint, int appendToPath);
int PHYSFS_removeFromSearchPath(const char *oldDir);
PHYSFS_File *PHYSFS_openRead(const char *filename);
int PHYSFS_exists(const char *filename);
int PHYSFS_eof(PHYSFS_File *file);
int64_t PHYSFS_readBytes(PHYSFS_File *file, void *buffer, uint64_t len);
int64_t PHYSFS_close(PHYSFS_File *file);
const char *PHYSFS_getLastError(void);
int64_t PHYSFS_fileLength(PHYSFS_File *file);
const char *PHYSFS_getRealDir(const char *filename);
char **PHYSFS_enumerateFiles(const char *dir);
void PHYSFS_freeList(void *listVar);

#ifdef __cplusplus
}
#endif

/* Helpers ported from PhysFS.pas. */

/* Reads the whole file into a malloc'd buffer; size returned via *outSize.
 * Returns NULL when missing. */
unsigned char *PHYSFS_readBuffer(const char *name, int *outSize);
/* Reads a single line (up to and excluding the newline) into buf. */
void PHYSFS_ReadLn(PHYSFS_File *handle, char *buf, int bufsize);
int PHYSFS_CopyFileFromArchive(const char *sourceFile, const char *destination);
/* Enumerates dir; returns a malloc'd array of strings, count via *outCount. */
char **PHYSFS_GetEnumeratedFiles(const char *dir, int *outCount);

#endif /* PHYSFS_PORT_H */
