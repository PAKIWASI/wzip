#include "wzip.h"


int main(void)
{
    WzDecompressor dcmpsr;
    wz_create_decompressor(&dcmpsr, "../deflate_spec.txt.gz");

    return 0;
}
