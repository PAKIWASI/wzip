#include "wzip.h"
#include "views_single.h"
#include <stdio.h>
#include <sys/mman.h>


void wz_create_decompressor(WzDecompressor* dcmpsr, strview filepath)
{
    dcmpsr->input_buf_size = WZ_DEFAULT_BUF_SIZE;
    dcmpsr->output_buf_size = WZ_DEFAULT_BUF_SIZE;

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
