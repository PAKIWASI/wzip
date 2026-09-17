#include "wzip.h"


int main(void)
{
    WzDecompressor dcmpsr;
    wz_create_decompressor(&dcmpsr, "/home/wasi/Projects/c/wzip/deflate_spec.txt.gz");
    inflate(&dcmpsr);

    return 0;
}
