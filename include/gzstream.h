#ifndef GZSTREAM_H
#define GZSTREAM_H

#include "common_single.h"
#include <stddef.h>


typedef struct {
    const u8* stream;  // pointer to the data (can by mmap ptr)
    u64       size;    // total bytes available
    u64       bytepos; // next byte to pull from
    u32       bitbuf;  // bits currently held
    int       bitcnt;  // how many bits are valid in bitbuf
    int       error;   // sticky error flag
} WzBitReader;



void wz_bitreader_create(WzBitReader* wzbr);

// LSB-first: DEFLATE's ordering for all values except Huffman codes

// look at n bits, don't consume
u32 wz_bitreader_peek(WzBitReader* wzbr, int n);

// consume n bits from the stream
void wz_bitreader_take(WzBitReader* wzbr, int n);

// peek + consume
u32 wz_bitreader_read(WzBitReader* wzbr, int n);

// discard bits to next byte boundary
void wz_bitreader_align_byte(WzBitReader* wzbr);


/*
The standard trick (zlib, libdeflate, miniz) is to pre-reverse the Huffman table at build time, not the data at decode time. You build your lookup table indexed by bit-reversed codes, so the hot loop becomes:

// bitbuf: next bits, MSB-first (bit bitcnt-1 is next on wire)
u16 idx = (u16)(bitbuf >> (bitcnt - FASTBITS));   // top FASTBITS bits
entry = table[idx];                               // no reversal anywhere

That shifts all reversal cost to table construction (once per block) and removes it from the per-symbol path entirely. This is what production decoders do.
*/

#endif // GZSTREAM_H
