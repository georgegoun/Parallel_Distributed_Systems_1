#ifndef HEADER_FILE_OPENMP
#define HEADER_FILE_OPENMP
#include <stdio.h>
#include <stdint.h>

int openmp_counting(uint32_t* csc_row,
    uint32_t* csc_col,
    uint32_t nz,
    uint32_t N);

#endif