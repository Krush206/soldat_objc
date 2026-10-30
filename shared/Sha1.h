/* Sha1 — C port of the FreePascal sha1 unit subset used by OpenSoldat. */

#ifndef SHA1_H
#define SHA1_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
	unsigned char data[20];
} TSHA1Digest;

void Sha1Buffer(const void *buffer, size_t len, TSHA1Digest *digest);
void Sha1String(const char *text, TSHA1Digest *digest);
/* FPC-style: hash a file in chunks of bufsize bytes. */
int Sha1File(const char *filename, int bufsize, TSHA1Digest *digest);
int Sha1Match(const TSHA1Digest *a, const TSHA1Digest *b);
char *Sha1DigestToString(const TSHA1Digest *digest);

#endif /* SHA1_H */
