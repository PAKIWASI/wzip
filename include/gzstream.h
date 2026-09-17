#ifndef GZSTREAM_H
#define GZSTREAM_H

#include "common_single.h"
#include <stddef.h>



typedef struct {
    const u8* data;
    size_t    size;    // total bytes available
    size_t    bytepos; // next byte to pull from
    u32       bitbuf;  // bits currently held
    int       bitcnt;  // how many bits are valid in bitbuf
    int       error;   // sticky error flag
} BitReader;

// LSB-first: DEFLATE's ordering for all values except Huffman codes
static inline u32  bitreader_peek(BitReader* br, int n); // look at n bits, don't consume
static inline void bitreader_consume(BitReader* br, int n);
static inline u32  bitreader_read(BitReader* br, int n); // peek + consume
static inline void bitreader_align_byte(BitReader* br);  // discard bits to next byte boundary


/*
The standard trick (zlib, libdeflate, miniz) is to pre-reverse the Huffman table at build time, not the data at decode time. You build your lookup table indexed by bit-reversed codes, so the hot loop becomes:

// bitbuf: next bits, MSB-first (bit bitcnt-1 is next on wire)
u16 idx = (u16)(bitbuf >> (bitcnt - FASTBITS));   // top FASTBITS bits
entry = table[idx];                               // no reversal anywhere


That shifts all reversal cost to table construction (once per block) and removes it from the per-symbol path entirely. This is what production decoders do. If you're optimizing, this is the win — not making reverse_u16 faster.
*/

#endif // GZSTREAM_H
