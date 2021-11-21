#ifndef HEADER_FILE_SEQ
#define HEADER_FILE_SEQ

#include <stdio.h>
#include <stdint.h>

int seq_counting(uint32_t*  csc_row,
    uint32_t*  csc_col,
    uint32_t  nz,
    uint32_t  N);

#endif