/* Md5 — C port of the FreePascal Md5 unit subset used by OpenSoldat. */

#include "Md5.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* MD5 context */
typedef struct {
    uint32_t state[4];
    uint32_t count[2];
    unsigned char buffer[64];
} MD5_CTX;

/* Constants for MD5Transform routine */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

/* F, G, H and I are basic MD5 functions */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4 */
#define FF(a, b, c, d, x, s, ac) { \
    (a) += F((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT((a), (s)); \
    (a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) { \
    (a) += G((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT((a), (s)); \
    (a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) { \
    (a) += H((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT((a), (s)); \
    (a) += (b); \
}
#define II(a, b, c, d, x, s, ac) { \
    (a) += I((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT((a), (s)); \
    (a) += (b); \
}

static void MD5Init(MD5_CTX *ctx) {
    ctx->count[0] = ctx->count[1] = 0;
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xefcdab89;
    ctx->state[2] = 0x98badcfe;
    ctx->state[3] = 0x10325476;
}

static void MD5Transform(uint32_t state[4], const unsigned char block[64]) {
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
    uint32_t x[16];

    /* Decode block into x */
    for (int i = 0, j = 0; i < 16; i++, j += 4) {
        x[i] = ((uint32_t)block[j]) |
               (((uint32_t)block[j + 1]) << 8) |
               (((uint32_t)block[j + 2]) << 16) |
               (((uint32_t)block[j + 3]) << 24);
    }

    /* Round 1 */
    FF(a, b, c, d, x[0],  S11, 0xd76aa478);
    FF(d, a, b, c, x[1],  S12, 0xe8c7b756);
    FF(c, d, a, b, x[2],  S13, 0x242070db);
    FF(b, c, d, a, x[3],  S14, 0xc1bdceee);
    FF(a, b, c, d, x[4],  S11, 0xf57c0faf);
    FF(d, a, b, c, x[5],  S12, 0x4787c62a);
    FF(c, d, a, b, x[6],  S13, 0xa8304613);
    FF(b, c, d, a, x[7],  S14, 0xfd469501);
    FF(a, b, c, d, x[8],  S11, 0x698098d8);
    FF(d, a, b, c, x[9],  S12, 0x8b44f7af);
    FF(c, d, a, b, x[10], S13, 0xffff5bb1);
    FF(b, c, d, a, x[11], S14, 0x895cd7be);
    FF(a, b, c, d, x[12], S11, 0x6b901122);
    FF(d, a, b, c, x[13], S12, 0xfd987193);
    FF(c, d, a, b, x[14], S13, 0xa679438e);
    FF(b, c, d, a, x[15], S14, 0x49b40821);

    /* Round 2 */
    GG(a, b, c, d, x[1],  S21, 0xf61e2562);
    GG(d, a, b, c, x[6],  S22, 0xc040b340);
    GG(c, d, a, b, x[11], S23, 0x265e5a51);
    GG(b, c, d, a, x[0],  S24, 0xe9b6c7aa);
    GG(a, b, c, d, x[5],  S21, 0xd62f105d);
    GG(d, a, b, c, x[10], S22, 0x02441453);
    GG(c, d, a, b, x[15], S23, 0xd8a1e681);
    GG(b, c, d, a, x[4],  S24, 0xe7d3fbc8);
    GG(a, b, c, d, x[9],  S21, 0x21e1cde6);
    GG(d, a, b, c, x[14], S22, 0xc33707d6);
    GG(c, d, a, b, x[3],  S23, 0xf4d50d87);
    GG(b, c, d, a, x[8],  S24, 0x455a14ed);
    GG(a, b, c, d, x[13], S21, 0xa9e3e905);
    GG(d, a, b, c, x[2],  S22, 0xfcefa3f8);
    GG(c, d, a, b, x[7],  S23, 0x676f02d9);
    GG(b, c, d, a, x[12], S24, 0x8d2a4c8a);

    /* Round 3 */
    HH(a, b, c, d, x[5],  S31, 0xfffa3942);
    HH(d, a, b, c, x[8],  S32, 0x8771f681);
    HH(c, d, a, b, x[11], S33, 0x6d9d6122);
    HH(b, c, d, a, x[14], S34, 0xfde5380c);
    HH(a, b, c, d, x[1],  S31, 0xa4beea44);
    HH(d, a, b, c, x[4],  S32, 0x4bdecfa9);
    HH(c, d, a, b, x[7],  S33, 0xf6bb4b60);
    HH(b, c, d, a, x[10], S34, 0xbebfbc70);
    HH(a, b, c, d, x[13], S31, 0x289b7ec6);
    HH(d, a, b, c, x[0],  S32, 0xeaa127fa);
    HH(c, d, a, b, x[3],  S33, 0xd4ef3085);
    HH(b, c, d, a, x[6],  S34, 0x04881d05);
    HH(a, b, c, d, x[9],  S31, 0xd9d4d039);
    HH(d, a, b, c, x[12], S32, 0xe6db99e5);
    HH(c, d, a, b, x[15], S33, 0x1fa27cf8);
    HH(b, c, d, a, x[2],  S34, 0xc4ac5665);

    /* Round 4 */
    II(a, b, c, d, x[0],  S41, 0xf4292244);
    II(d, a, b, c, x[7],  S42, 0x432aff97);
    II(c, d, a, b, x[14], S43, 0xab9423a7);
    II(b, c, d, a, x[5],  S44, 0xfc93a039);
    II(a, b, c, d, x[12], S41, 0x655b59c3);
    II(d, a, b, c, x[3],  S42, 0x8f0ccc92);
    II(c, d, a, b, x[10], S43, 0xffeff47d);
    II(b, c, d, a, x[1],  S44, 0x85845dd1);
    II(a, b, c, d, x[8],  S41, 0x6fa87e4f);
    II(d, a, b, c, x[15], S42, 0xfe2ce6e0);
    II(c, d, a, b, x[6],  S43, 0xa3014314);
    II(b, c, d, a, x[13], S44, 0x4e0811a1);
    II(a, b, c, d, x[4],  S41, 0xf7537e82);
    II(d, a, b, c, x[11], S42, 0xbd3af235);
    II(c, d, a, b, x[2],  S43, 0x2ad7d2bb);
    II(b, c, d, a, x[9],  S44, 0xeb86d391);

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

static void MD5Update(MD5_CTX *ctx, const unsigned char *input, size_t len) {
    uint32_t i, index, partLen;

    index = (ctx->count[0] >> 3) & 0x3F;

    if ((ctx->count[0] += ((uint32_t)len << 3)) < ((uint32_t)len << 3))
        ctx->count[1]++;
    ctx->count[1] += ((uint32_t)len >> 29);

    partLen = 64 - index;

    if (len >= partLen) {
        memcpy(&ctx->buffer[index], input, partLen);
        MD5Transform(ctx->state, ctx->buffer);

        for (i = partLen; i + 63 < len; i += 64)
            MD5Transform(ctx->state, &input[i]);

        index = 0;
    } else {
        i = 0;
    }

    memcpy(&ctx->buffer[index], &input[i], len - i);
}

static void MD5Final(unsigned char digest[16], MD5_CTX *ctx) {
    unsigned char bits[8];
    uint32_t index, padLen;

    /* Save number of bits */
    for (int i = 0; i < 8; i++) {
        bits[i] = (unsigned char)((ctx->count[i >> 2] >> ((i & 3) << 3)) & 0xFF);
    }

    /* Pad out to 56 mod 64 */
    index = (ctx->count[0] >> 3) & 0x3F;
    padLen = (index < 56) ? (56 - index) : (120 - index);
    static unsigned char padding[64] = {0x80};
    MD5Update(ctx, padding, padLen);

    /* Append length */
    MD5Update(ctx, bits, 8);

    /* Store state in digest */
    for (int i = 0, j = 0; i < 4; i++, j += 4) {
        digest[j]     = (unsigned char)(ctx->state[i] & 0xFF);
        digest[j + 1] = (unsigned char)((ctx->state[i] >> 8) & 0xFF);
        digest[j + 2] = (unsigned char)((ctx->state[i] >> 16) & 0xFF);
        digest[j + 3] = (unsigned char)((ctx->state[i] >> 24) & 0xFF);
    }
}

void MD5Buffer(const void *buffer, size_t len, TMD5Digest *digest) {
    MD5_CTX ctx;
    MD5Init(&ctx);
    MD5Update(&ctx, (const unsigned char *)buffer, len);
    MD5Final(digest->data, &ctx);
}

void MD5String(const char *text, TMD5Digest *digest) {
    MD5Buffer(text, strlen(text), digest);
}

char *MD5Print(const TMD5Digest *digest) {
    static const char hex[] = "0123456789abcdef";
    char *str = malloc(33);
    if (!str) return NULL;

    for (int i = 0; i < 16; i++) {
        str[i * 2]     = hex[(digest->data[i] >> 4) & 0xF];
        str[i * 2 + 1] = hex[digest->data[i] & 0xF];
    }
    str[32] = '\0';
    return str;
}

int MD5Match(const TMD5Digest *a, const TMD5Digest *b) {
    return memcmp(a->data, b->data, 16) == 0;
}