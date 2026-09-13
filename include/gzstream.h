#ifndef GZSTREAM_H
#define GZSTREAM_H

#include "common_single.h"


static inline u16 read_u16le(const u8* p)
{
    return (u16)p[0]           // low 8 bits set to first bit from p
           | ((u16)p[1] << 8); // high 8 bits set to second bit from p
}

static inline u32 read_u32le(const u8* p)
{
    return p[0]                 // lowest 8 bits set
           | ((u32)p[1] << 8)   // next 8 bytes
           | ((u32)p[2] << 16)  // next 8 bytes
           | ((u32)p[3] << 24); // highest 8 bytes
}

static inline u8 reverse_u8(u8 x)
{
    // swap adjacent bits. 0x55 = 0b01010101, you get even positioned bits
    // first op takes bits from positions 1,3,5,7 and moves them one position to the right.
    // second op takes bits from positions 0,2,4,6 and moves them one position left.
    // so we are swapping pairs of bits (all 2 bit pairs are swapped)
    x = ((x >> 1) & 0x55) | ((x & 0x55) << 1);
    // this swaps groups of 2 bits. 0x33 = 0b00110011,
    x = ((x >> 2) & 0x33) | ((x & 0x33) << 2);
    // this swaps nibbles (groups of 4 bits), 4 = 0b00001111
    x = (x >> 4) | (x << 4);
    return x;
}

static inline u16 reverse_u16(const u8* p)
{
    return ((u16)
            ((u16)reverse_u8(p[0]) << 8)    // write lower 8 bytes from p into high 8 bytes of output, reversed
            | (reverse_u8(p[1])));          // write higher 8 bytes from p into lower 8 bytes of output, reversed
}


// TODO: should i use ptr input or just input by value?

#endif // GZSTREAM_H
