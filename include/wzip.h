#ifndef WZIP_H
#define WZIP_H

#include "views_single.h"


/*
 * Gzip FLG byte bit flags (RFC 1952, section 2.3.1).
 * Set bits indicate which optional header fields are present
 * after the fixed 10-byte header. Bits 5..7 are reserved and
 * MUST be zero in a conforming stream.
 */
typedef enum {
    FTEXT    = 1 << 0, /* file is probably ASCII text (hint only)      */
    FHCRC    = 1 << 1, /* a 16-bit header CRC (crc16) follows          */
    FEXTRA   = 1 << 2, /* an "extra field" region (xlen bytes) follows */
    FNAME    = 1 << 3, /* a NUL-terminated original filename follows   */
    FCOMMENT = 1 << 4, /* a NUL-terminated comment follows             */
} GzFlags;


/*
 * Gzip State Machine
*/
typedef enum {
    WzNOT_RUNNING = 0,
    WzREAD_HEADER,
    WzREAD_BLOCKS,
    wzREAD_TRAILER,
} WzState;


/*
 * On-disk fixed gzip header, exactly 10 bytes (RFC 1952, section 2.3.1).
 * Read this verbatim from the stream
 * Any optional fields (extra, name, comment, header CRC) follow this
 * struct in the stream and are handled separately by the parser.
 */
#pragma pack(push, 1)
typedef struct {
    u8  id1, id2; /* magic bytes: must be 0x1f, 0x8b                         */
    u8  cm;       /* compression method: 8 = DEFLATE                         */
    u8  flg;      /* flag bits: bitmask of GzFlags                           */
    u32 mtime;    /* modification time, Unix epoch seconds (0 = unavailable) */
    u8  xflg;     /* extra flags; DEFLATE: 2 = max, 4 = fastest              */
    u8  os;       /* originating OS code (0=FAT, 3=Unix, 255=unknown, ...)   */
} GzHeader;
#pragma pack(pop)

/*
 * On-disk gzip trailer, exactly 8 bytes (RFC 1952, section 2.3.2).
 * Follows the compressed payload.
 */
#pragma pack(push, 1)
typedef struct {
    u32 crc32; /* CRC-32 of the uncompressed data */
    u32 isize; /* uncompressed size, modulo 2^32  */
} GzTrailer;
#pragma pack(pop)


/*
 * Compile-time sanity checks: these structs must match the wire format.
 */
#if defined(__cplusplus)
static_assert(sizeof(GzHeader) == 10, "GzHeader must be 10 bytes");
static_assert(sizeof(GzTrailer) == 8, "GzTrailer must be 8 bytes");
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
_Static_assert(sizeof(GzHeader) == 10, "GzHeader must be 10 bytes");
_Static_assert(sizeof(GzTrailer) == 8, "GzTrailer must be 8 bytes");
#endif


#define WZ_DEFAULT_BUF_SIZE (nKB(64))

/*
 * The wzip decompressor streams (mmap) data from the gzip file and processes it in 64KB (default) chunks
 * A state machine is used for management and huffman state is stored if one chunk doesnot finish a
 * member. The output buffer is also 64KB and writes to the file when it's full or when we reach the end of file.
 * We don't care about individual members or extra metadata in this decompression
 */
typedef struct {
    // decompression configuration
    u32 input_buf_size;
    u32 output_buf_size;
    // data about the current member we are reading
    GzFlags   flags;
    WzState   state;
    GzHeader  header;
    u8        _[6];
    GzTrailer trailer;
    // current file we are processing
    int fd;
    // data about the decomresssion
    u32 num_members;
    u64 compressed_data_bytes;
    u64 uncompressed_data_bytes;
} WzDecompressor;


void wz_create_decompressor(WzDecompressor* dcmpsr, const char* filepath);

void inflate(WzDecompressor* dcmpsr);


/*
┌─────────────────────────────────────────────────────────────────┐
│                        GZIP MEMBER                              │
├─────────────────────────────────────────────────────────────────┤
│ FIXED HEADER (10 bytes)                                         │
│                                                                 │
│   off  size  field                                              │
│   0    1     ID1     = 0x1f                                     │
│   1    1     ID2     = 0x8b                                     │
│   2    1     CM      = 8 (DEFLATE)                              │
│   3    1     FLG     bit flags (FTEXT, FHCRC, FEXTRA,           │
│                      FNAME, FCOMMENT)                           │
│   4    4     MTIME   u32 LE, Unix seconds                       │
│   8    1     XFLG    extra flags                                │
│   9    1     OS      OS code                                    │
├─────────────────────────────────────────────────────────────────┤
│ OPTIONAL FIELDS: present only if the matching FLG bit is set,   │
│ and ALWAYS in this order:                                       │
│                                                                 │
│   if FEXTRA  : XLEN (u16 LE) + XLEN bytes of extra data         │
│   if FNAME   : filename bytes + 0x00 terminator                 │
│   if FCOMMENT: comment bytes  + 0x00 terminator                 │
│   if FHCRC   : HCRC  (u16 LE)                                   │
│   (we skip fextra, fname, fcomment)                             │
├─────────────────────────────────────────────────────────────────┤
│ COMPRESSED DATA: raw DEFLATE stream, length not stored          │
├─────────────────────────────────────────────────────────────────┤
│ TRAILER (8 bytes)                                               │
│                                                                 │
│   CRC32  u32 LE  CRC of uncompressed data                       │
│   ISIZE  u32 LE  uncompressed size, mod 2^32                    │
└─────────────────────────────────────────────────────────────────┘
*/

#endif // WZIP_H
