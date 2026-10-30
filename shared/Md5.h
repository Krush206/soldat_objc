/* Md5 — C port of the FreePascal Md5 unit subset used by OpenSoldat. */

#ifndef MD5_H
#define MD5_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
	unsigned char data[16];
} TMD5Digest;

void MD5Buffer(const void *buffer, size_t len, TMD5Digest *digest);
void MD5String(const char *text, TMD5Digest *digest);
/* Lowercase hex representation, as FPC's MD5Print. */
char *MD5Print(const TMD5Digest *digest);
int MD5Match(const TMD5Digest *a, const TMD5Digest *b);

#endif /* MD5_H */
