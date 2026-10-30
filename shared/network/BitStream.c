/* BitStream — C port of shared/libs/BitStream.pas */
#include "BitStream.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

/* --- BitReader implementation --- */

BitReader *BitReader_Create(uint8_t *buffer, unsigned int bufferBytes) {
    BitReader *br = calloc(1, sizeof(BitReader));
    if (!br) return NULL;
    br->FBuffer = buffer;
    br->FBufferBytes = (int)bufferBytes;
    br->FBitPos = 0;
    return br;
}

void BitReader_Free(BitReader *br) {
    if (!br) return;
    free(br);
}

static uint64_t BitReader_ReadBitsInternal(BitReader *br, int nbits) {
    if (!br || nbits <= 0 || nbits > 64) return 0;
    if (br->FBitPos + nbits > br->FBufferBytes * 8) return 0;
    
    uint64_t result = 0;
    for (int i = 0; i < nbits; i++) {
        int byteIndex = br->FBitPos >> 3;
        int bitIndex = br->FBitPos & 7;
        if (br->FBuffer[byteIndex] & (1 << bitIndex)) {
            result |= (1ULL << i);
        }
        br->FBitPos++;
    }
    return result;
}

uint8_t BitReader_ReadUInt8(BitReader *br) {
    return (uint8_t)BitReader_ReadBitsInternal(br, 8);
}

uint8_t BitReader_ReadUInt8Bits(BitReader *br, int nbits) {
    return (uint8_t)BitReader_ReadBitsInternal(br, nbits);
}

uint16_t BitReader_ReadUInt16(BitReader *br) {
    return (uint16_t)BitReader_ReadBitsInternal(br, 16);
}

uint16_t BitReader_ReadUInt16Bits(BitReader *br, int nbits) {
    return (uint16_t)BitReader_ReadBitsInternal(br, nbits);
}

uint32_t BitReader_ReadUInt32(BitReader *br) {
    return (uint32_t)BitReader_ReadBitsInternal(br, 32);
}

uint32_t BitReader_ReadUInt32Bits(BitReader *br, int nbits) {
    return (uint32_t)BitReader_ReadBitsInternal(br, nbits);
}

int8_t BitReader_ReadInt8(BitReader *br) {
    return (int8_t)BitReader_ReadUInt8(br);
}

int8_t BitReader_ReadInt8Bits(BitReader *br, int nbits) {
    return (int8_t)BitReader_ReadUInt8Bits(br, nbits);
}

int16_t BitReader_ReadInt16(BitReader *br) {
    return (int16_t)BitReader_ReadUInt16(br);
}

int16_t BitReader_ReadInt16Bits(BitReader *br, int nbits) {
    return (int16_t)BitReader_ReadUInt16Bits(br, nbits);
}

int32_t BitReader_ReadInt32(BitReader *br) {
    return (int32_t)BitReader_ReadUInt32(br);
}

int32_t BitReader_ReadInt32Bits(BitReader *br, int nbits) {
    return (int32_t)BitReader_ReadUInt32Bits(br, nbits);
}

float BitReader_ReadSingle(BitReader *br) {
    union {
        uint32_t u;
        float f;
    } u;
    u.u = BitReader_ReadUInt32(br);
    return u.f;
}

bool BitReader_ReadBoolean(BitReader *br) {
    return BitReader_ReadUInt8(br) != 0;
}

char *BitReader_ReadString(BitReader *br) {
    /* Read length-prefixed string */
    uint16_t len = BitReader_ReadUInt16(br);
    if (len == 0) return strdup("");
    
    if (br->FBitPos + len * 8 > br->FBufferBytes * 8) return strdup("");
    
    char *str = malloc(len + 1);
    if (!str) return strdup("");
    
    for (uint16_t i = 0; i < len; i++) {
        str[i] = (char)BitReader_ReadUInt8(br);
    }
    str[len] = '\0';
    return str;
}

char *BitReader_ReadStringLen(BitReader *br, int len) {
    if (len <= 0) return strdup("");
    if (br->FBitPos + len * 8 > br->FBufferBytes * 8) return strdup("");
    
    char *str = malloc(len + 1);
    if (!str) return strdup("");
    
    for (int i = 0; i < len; i++) {
        str[i] = (char)BitReader_ReadUInt8(br);
    }
    str[len] = '\0';
    return str;
}

void BitReader_Skip(BitReader *br, int nbits) {
    if (!br) return;
    br->FBitPos += nbits;
}

int BitReader_GetBitPos(const BitReader *br) {
    return br ? br->FBitPos : 0;
}

void BitReader_SetBitPos(BitReader *br, int bitPos) {
    if (br) br->FBitPos = bitPos;
}

uint8_t *BitReader_GetBuffer(const BitReader *br) {
    return br ? br->FBuffer : NULL;
}

/* --- BitWriter implementation --- */

BitWriter *BitWriter_Create(unsigned int bufferBytes) {
    BitWriter *bw = calloc(1, sizeof(BitWriter));
    if (!bw) return NULL;
    
    bw->FBuffersCapacity = 16;
    bw->FBuffers = calloc(bw->FBuffersCapacity, sizeof(uint8_t *));
    if (!bw->FBuffers) {
        free(bw);
        return NULL;
    }
    
    bw->FBufferBytes = (int)bufferBytes;
    bw->FBuffer = calloc(1, bufferBytes);
    if (!bw->FBuffer) {
        free(bw->FBuffers);
        free(bw);
        return NULL;
    }
    
    bw->FBuffers[bw->FBuffersCount++] = bw->FBuffer;
    bw->FBitPos = 0;
    return bw;
}

void BitWriter_Free(BitWriter *bw) {
    if (!bw) return;
    for (int i = 0; i < bw->FBuffersCount; i++) {
        free(bw->FBuffers[i]);
    }
    free(bw->FBuffers);
    free(bw);
}

static void BitWriter_EnsureCapacity(BitWriter *bw, int neededBits) {
    if (!bw) return;
    int availableBits = bw->FBufferBytes * 8 - bw->FBitPos;
    if (availableBits >= neededBits) return;
    
    /* Add a new buffer */
    if (bw->FBuffersCount >= bw->FBuffersCapacity) {
        bw->FBuffersCapacity *= 2;
        bw->FBuffers = realloc(bw->FBuffers, bw->FBuffersCapacity * sizeof(uint8_t *));
    }
    
    bw->FBuffer = calloc(1, bw->FBufferBytes);
    bw->FBuffers[bw->FBuffersCount++] = bw->FBuffer;
    bw->FBitPos = 0;
}

static void BitWriter_WriteBitsInternal(BitWriter *bw, uint64_t val, int nbits) {
    if (!bw || nbits <= 0 || nbits > 64) return;
    BitWriter_EnsureCapacity(bw, nbits);
    
    for (int i = 0; i < nbits; i++) {
        int byteIndex = bw->FBitPos >> 3;
        int bitIndex = bw->FBitPos & 7;
        if (val & (1ULL << i)) {
            bw->FBuffer[byteIndex] |= (1 << bitIndex);
        }
        bw->FBitPos++;
    }
}

void BitWriter_CloneBuffer(BitWriter *bw, uint8_t **buffer, unsigned int *sizeInBytes) {
    if (!bw || !buffer || !sizeInBytes) return;
    
    int totalBits = 0;
    for (int i = 0; i < bw->FBuffersCount; i++) {
        int bits = (i == bw->FBuffersCount - 1) ? bw->FBitPos : bw->FBufferBytes * 8;
        totalBits += bits;
    }
    
    int totalBytes = (totalBits + 7) / 8;
    *buffer = malloc(totalBytes);
    *sizeInBytes = (unsigned int)totalBytes;
    
    if (!*buffer) return;
    
    uint8_t *dst = *buffer;
    int dstBit = 0;
    for (int i = 0; i < bw->FBuffersCount; i++) {
        int bits = (i == bw->FBuffersCount - 1) ? bw->FBitPos : bw->FBufferBytes * 8;
        for (int b = 0; b < bits; b++) {
            int srcByte = b >> 3;
            int srcBit = b & 7;
            if (bw->FBuffers[i][srcByte] & (1 << srcBit)) {
                dst[dstBit >> 3] |= (1 << (dstBit & 7));
            }
            dstBit++;
        }
    }
}

void BitWriter_WriteUInt8(BitWriter *bw, uint8_t val) {
    BitWriter_WriteUInt8Bits(bw, val, 8);
}

void BitWriter_WriteUInt8Bits(BitWriter *bw, uint8_t val, int nbits) {
    BitWriter_WriteBitsInternal(bw, val, nbits);
}

void BitWriter_WriteUInt16(BitWriter *bw, uint16_t val) {
    BitWriter_WriteUInt16Bits(bw, val, 16);
}

void BitWriter_WriteUInt16Bits(BitWriter *bw, uint16_t val, int nbits) {
    BitWriter_WriteBitsInternal(bw, val, nbits);
}

void BitWriter_WriteUInt32(BitWriter *bw, uint32_t val) {
    BitWriter_WriteUInt32Bits(bw, val, 32);
}

void BitWriter_WriteUInt32Bits(BitWriter *bw, uint32_t val, int nbits) {
    BitWriter_WriteBitsInternal(bw, val, nbits);
}

void BitWriter_WriteInt8(BitWriter *bw, int8_t val) {
    BitWriter_WriteInt8Bits(bw, val, 8);
}

void BitWriter_WriteInt8Bits(BitWriter *bw, int8_t val, int nbits) {
    BitWriter_WriteBitsInternal(bw, (uint64_t)(val & 0xFF), nbits);
}

void BitWriter_WriteInt16(BitWriter *bw, int16_t val) {
    BitWriter_WriteInt16Bits(bw, val, 16);
}

void BitWriter_WriteInt16Bits(BitWriter *bw, int16_t val, int nbits) {
    BitWriter_WriteBitsInternal(bw, (uint64_t)(val & 0xFFFF), nbits);
}

void BitWriter_WriteInt32(BitWriter *bw, int32_t val) {
    BitWriter_WriteInt32Bits(bw, val, 32);
}

void BitWriter_WriteInt32Bits(BitWriter *bw, int32_t val, int nbits) {
    BitWriter_WriteBitsInternal(bw, (uint64_t)(val & 0xFFFFFFFFULL), nbits);
}

void BitWriter_WriteSingle(BitWriter *bw, float val) {
    union {
        uint32_t u;
        float f;
    } u;
    u.f = val;
    BitWriter_WriteUInt32(bw, u.u);
}

void BitWriter_WriteBoolean(BitWriter *bw, bool val) {
    BitWriter_WriteUInt8(bw, val ? 1 : 0);
}

void BitWriter_WriteString(BitWriter *bw, const char *val) {
    if (!val) {
        BitWriter_WriteUInt16(bw, 0);
        return;
    }
    size_t len = strlen(val);
    if (len > 65535) len = 65535;
    BitWriter_WriteUInt16(bw, (uint16_t)len);
    for (size_t i = 0; i < len; i++) {
        BitWriter_WriteUInt8(bw, (uint8_t)val[i]);
    }
}

void BitWriter_WriteStringLen(BitWriter *bw, const char *val, int len) {
    if (len <= 0 || !val) {
        BitWriter_WriteUInt16(bw, 0);
        return;
    }
    BitWriter_WriteUInt16(bw, (uint16_t)len);
    for (int i = 0; i < len; i++) {
        BitWriter_WriteUInt8(bw, (uint8_t)val[i]);
    }
}

int BitWriter_GetBitPos(const BitWriter *bw) {
    int pos = 0;
    for (int i = 0; i < bw->FBuffersCount; i++) {
        pos += (i == bw->FBuffersCount - 1) ? bw->FBitPos : bw->FBufferBytes * 8;
    }
    return pos;
}

/* --- Internal helpers for generic read/write --- */

uint64_t BitReader_ReadBits(BitReader *br, int nbits) {
    return BitReader_ReadBitsInternal(br, nbits);
}

void BitWriter_WriteBits(BitWriter *bw, uint64_t val, int nbits) {
    BitWriter_WriteBitsInternal(bw, val, nbits);
}