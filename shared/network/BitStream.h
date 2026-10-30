/* BitStream — C port of shared/libs/BitStream.pas */

#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>
#include <stdbool.h>

/* BitReader — reads bits from a buffer */
typedef struct BitReader {
    uint8_t *FBuffer;
    int FBufferBytes;
    int FBitPos;
} BitReader;

BitReader *BitReader_Create(uint8_t *buffer, unsigned int bufferBytes);
void BitReader_Free(BitReader *br);
uint8_t BitReader_ReadUInt8(BitReader *br);
uint8_t BitReader_ReadUInt8Bits(BitReader *br, int nbits);
uint16_t BitReader_ReadUInt16(BitReader *br);
uint16_t BitReader_ReadUInt16Bits(BitReader *br, int nbits);
uint32_t BitReader_ReadUInt32(BitReader *br);
uint32_t BitReader_ReadUInt32Bits(BitReader *br, int nbits);
int8_t BitReader_ReadInt8(BitReader *br);
int8_t BitReader_ReadInt8Bits(BitReader *br, int nbits);
int16_t BitReader_ReadInt16(BitReader *br);
int16_t BitReader_ReadInt16Bits(BitReader *br, int nbits);
int32_t BitReader_ReadInt32(BitReader *br);
int32_t BitReader_ReadInt32Bits(BitReader *br, int nbits);
float BitReader_ReadSingle(BitReader *br);
bool BitReader_ReadBoolean(BitReader *br);
char *BitReader_ReadString(BitReader *br);
char *BitReader_ReadStringLen(BitReader *br, int len);
void BitReader_Skip(BitReader *br, int nbits);
int BitReader_GetBitPos(const BitReader *br);
void BitReader_SetBitPos(BitReader *br, int bitPos);
uint8_t *BitReader_GetBuffer(const BitReader *br);

/* BitWriter — writes bits to a dynamically growing buffer */
typedef struct BitWriter {
    uint8_t **FBuffers;
    int FBuffersCount;
    int FBuffersCapacity;
    uint8_t *FBuffer;
    int FBufferBytes;
    int FBitPos;
} BitWriter;

BitWriter *BitWriter_Create(unsigned int bufferBytes);
void BitWriter_Free(BitWriter *bw);
void BitWriter_CloneBuffer(BitWriter *bw, uint8_t **buffer, unsigned int *sizeInBytes);
void BitWriter_WriteUInt8(BitWriter *bw, uint8_t val);
void BitWriter_WriteUInt8Bits(BitWriter *bw, uint8_t val, int nbits);
void BitWriter_WriteUInt16(BitWriter *bw, uint16_t val);
void BitWriter_WriteUInt16Bits(BitWriter *bw, uint16_t val, int nbits);
void BitWriter_WriteUInt32(BitWriter *bw, uint32_t val);
void BitWriter_WriteUInt32Bits(BitWriter *bw, uint32_t val, int nbits);
void BitWriter_WriteInt8(BitWriter *bw, int8_t val);
void BitWriter_WriteInt8Bits(BitWriter *bw, int8_t val, int nbits);
void BitWriter_WriteInt16(BitWriter *bw, int16_t val);
void BitWriter_WriteInt16Bits(BitWriter *bw, int16_t val, int nbits);
void BitWriter_WriteInt32(BitWriter *bw, int32_t val);
void BitWriter_WriteInt32Bits(BitWriter *bw, int32_t val, int nbits);
void BitWriter_WriteSingle(BitWriter *bw, float val);
void BitWriter_WriteBoolean(BitWriter *bw, bool val);
void BitWriter_WriteString(BitWriter *bw, const char *val);
void BitWriter_WriteStringLen(BitWriter *bw, const char *val, int len);
int BitWriter_GetBitPos(const BitWriter *bw);

/* Internal helpers for generic read/write (Pascal TBitRW<T>) */
uint64_t BitReader_ReadBits(BitReader *br, int nbits);
void BitWriter_WriteBits(BitWriter *bw, uint64_t val, int nbits);

#endif /* BITSTREAM_H */