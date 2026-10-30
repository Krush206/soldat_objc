#include "Sha1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHA1_ROTL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

static void Sha1Transform(uint32_t state[5], const unsigned char buffer[64]) {
    uint32_t a, b, c, d, e;
    uint32_t w[80];
    int i;

    for (i = 0; i < 16; i++) {
        w[i] = ((uint32_t)buffer[i * 4] << 24) |
               ((uint32_t)buffer[i * 4 + 1] << 16) |
               ((uint32_t)buffer[i * 4 + 2] << 8) |
               ((uint32_t)buffer[i * 4 + 3]);
    }
    for (i = 16; i < 80; i++) {
        w[i] = SHA1_ROTL(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
    }

    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];

    for (i = 0; i < 20; i++) {
        uint32_t t = SHA1_ROTL(a, 5) + ((b & c) | (~b & d)) + e + w[i] + 0x5A827999;
        e = d;
        d = c;
        c = SHA1_ROTL(b, 30);
        b = a;
        a = t;
    }
    for (i = 20; i < 40; i++) {
        uint32_t t = SHA1_ROTL(a, 5) + (b ^ c ^ d) + e + w[i] + 0x6ED9EBA1;
        e = d;
        d = c;
        c = SHA1_ROTL(b, 30);
        b = a;
        a = t;
    }
    for (i = 40; i < 60; i++) {
        uint32_t t = SHA1_ROTL(a, 5) + ((b & c) | (b & d) | (c & d)) + e + w[i] + 0x8F1BBCDC;
        e = d;
        d = c;
        c = SHA1_ROTL(b, 30);
        b = a;
        a = t;
    }
    for (i = 60; i < 80; i++) {
        uint32_t t = SHA1_ROTL(a, 5) + (b ^ c ^ d) + e + w[i] + 0xCA62C1D6;
        e = d;
        d = c;
        c = SHA1_ROTL(b, 30);
        b = a;
        a = t;
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

void Sha1Buffer(const void *buffer, size_t len, TSHA1Digest *digest) {
    uint32_t state[5] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };
    const unsigned char *data = (const unsigned char *)buffer;
    size_t i, index, partLen;

    while (len >= 64) {
        Sha1Transform(state, data);
        data += 64;
        len -= 64;
    }

    index = 0;
    while (len--) {
        unsigned char temp[64];
        memcpy(temp, data, len);
        temp[len] = 0x80;
        memset(temp + len + 1, 0, 63 - len);
        if (len < 56) {
            temp[63] = (unsigned char)(index * 8);
            temp[62] = (unsigned char)((index * 8) >> 8);
            temp[61] = (unsigned char)((index * 8) >> 16);
            temp[60] = (unsigned char)((index * 8) >> 24);
            Sha1Transform(state, temp);
        }
        data++;
        index++;
    }

    if (len == 0) {
        unsigned char temp[64];
        memset(temp, 0, 64);
        temp[0] = 0x80;
        temp[63] = (unsigned char)(index * 8);
        temp[62] = (unsigned char)((index * 8) >> 8);
        temp[61] = (unsigned char)((index * 8) >> 16);
        temp[60] = (unsigned char)((index * 8) >> 24);
        Sha1Transform(state, temp);
    }

    for (i = 0; i < 5; i++) {
        digest->data[i * 4] = (unsigned char)(state[i] >> 24);
        digest->data[i * 4 + 1] = (unsigned char)(state[i] >> 16);
        digest->data[i * 4 + 2] = (unsigned char)(state[i] >> 8);
        digest->data[i * 4 + 3] = (unsigned char)(state[i]);
    }
}

void Sha1String(const char *text, TSHA1Digest *digest) {
    Sha1Buffer(text, strlen(text), digest);
}

int Sha1File(const char *filename, int bufsize, TSHA1Digest *digest) {
    FILE *file;
    unsigned char *buffer;
    uint32_t state[5] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };
    size_t bytesRead;
    int ret = 0;

    if (bufsize <= 0)
        bufsize = 4096;

    buffer = (unsigned char *)malloc(bufsize + 64);
    if (!buffer)
        return -1;

    file = fopen(filename, "rb");
    if (!file) {
        free(buffer);
        return -1;
    }

    while ((bytesRead = fread(buffer, 1, bufsize, file)) > 0) {
        size_t i;
        for (i = 0; i + 64 <= bytesRead; i += 64) {
            Sha1Transform(state, buffer + i);
        }
        if (i < bytesRead) {
            size_t remaining = bytesRead - i;
            memmove(buffer, buffer + i, remaining);
            if (remaining < 56) {
                buffer[remaining] = 0x80;
                memset(buffer + remaining + 1, 0, 55 - remaining);
                buffer[63] = (unsigned char)(0);
                buffer[62] = (unsigned char)(0);
                buffer[61] = (unsigned char)(0);
                buffer[60] = (unsigned char)(0);
                Sha1Transform(state, buffer);
                memset(buffer, 0, 64);
            } else {
                buffer[remaining] = 0x80;
                memset(buffer + remaining + 1, 0, 63 - remaining);
            }
        }
    }

    if (feof(file)) {
        unsigned char finalBlock[64];
        memset(finalBlock, 0, 64);
        finalBlock[0] = 0x80;
        finalBlock[63] = 0;
        finalBlock[62] = 0;
        finalBlock[61] = 0;
        finalBlock[60] = 0;
        Sha1Transform(state, finalBlock);
        ret = 1;
    }

    fclose(file);
    free(buffer);

    if (ret) {
        for (int i = 0; i < 5; i++) {
            digest->data[i * 4] = (unsigned char)(state[i] >> 24);
            digest->data[i * 4 + 1] = (unsigned char)(state[i] >> 16);
            digest->data[i * 4 + 2] = (unsigned char)(state[i] >> 8);
            digest->data[i * 4 + 3] = (unsigned char)(state[i]);
        }
    }

    return ret;
}

int Sha1Match(const TSHA1Digest *a, const TSHA1Digest *b) {
    return memcmp(a->data, b->data, 20) == 0;
}

char *Sha1DigestToString(const TSHA1Digest *digest) {
    static const char hex[] = "0123456789abcdef";
    char *str = (char *)malloc(41);
    if (!str)
        return NULL;
    for (int i = 0; i < 20; i++) {
        str[i * 2] = hex[digest->data[i] >> 4];
        str[i * 2 + 1] = hex[digest->data[i] & 0xF];
    }
    str[40] = '\0';
    return str;
}