#include "wzip.h"
#include "views_single.h"
#include <fcntl.h>
#include <stdalign.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>


void wz_create_decompressor(WzDecompressor* dcmpsr, const char* filepath)
{
    dcmpsr->input_buf_size  = WZ_DEFAULT_BUF_SIZE;
    dcmpsr->output_buf_size = WZ_DEFAULT_BUF_SIZE;

    dcmpsr->flags = 0;
    dcmpsr->state = WzD_NOT_STARTED;
    memset(&dcmpsr->header, 0, sizeof(GzHeader));
    memset(&dcmpsr->trailer, 0, sizeof(GzTrailer));

    dcmpsr->fd = open(filepath, O_RDONLY);
    CHECK_FATAL(dcmpsr->fd < 0, "could not open file %s", filepath);

    dcmpsr->num_members             = 0;
    dcmpsr->compressed_data_bytes   = 0;
    dcmpsr->uncompressed_data_bytes = 0;
}

void inflate(WzDecompressor* dcmpsr)
{
    u8* data = mmap(NULL,                   // let kernel choose address
                    dcmpsr->input_buf_size, // number of bytes to map
                    PROT_READ,              // read-only
                    MAP_PRIVATE,            // private mapping
                    dcmpsr->fd,             // file descriptor
                    0                       // file offset
    );
    if (data == MAP_FAILED) {
        perror("mmap");
        return;
    }


    printf("%02x\n", data[0]);
    printf("%02x\n", data[1]);
    printf("%02x\n", data[2]);

    printf("%b\n", data[3]);

    printf("%d\n", *(int*)(data + 4));

    printf("%b\n", data[8]);

    printf("%b\n", data[9]);

    const char* fname = (const char*)(data + 10);
    printf("%s\n", fname);

    u8* blocks = (data + 10 + strlen(fname));

    u8* curr = blocks + (7 & ~7);
    while (*curr != 0) {
        curr = (curr + 1) + (7 & ~7);
    }

    u32 isize = *(u32*)(curr - 8 - 4);
    u32 crc32 = *(u32*)(curr - 8 - 8);

    printf("%d\n", crc32);
    printf("%d\n", isize);
}
