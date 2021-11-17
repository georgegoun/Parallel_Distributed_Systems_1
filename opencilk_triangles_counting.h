#ifndef HEADER_FILE_OPENCILK
#define HEADER_FILE_OPENCILK

#include <stdio.h>
#include <stdint.h>

int seq_counting(uint32_t* csc_row,
    uint32_t* csc_col,
    uint32_t nz,
    uint32_t N);

#endif