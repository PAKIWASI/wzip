#include "wzip.h"
#include "views_single.h"
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>


void wz_create_decompressor(WzDecompressor* dcmpsr, const char* filepath)
{
    dcmpsr->input_buf_size  = WZ_DEFAULT_BUF_SIZE;
    dcmpsr->output_buf_size = WZ_DEFAULT_BUF_SIZE;

    dcmpsr->flags = 0;
    dcmpsr->state = WzNOT_RUNNING;
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
}
