#include "openmp_triangles_counting.h"
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int openmp_counting(uint32_t* csc_row,
    uint32_t* csc_col,
    uint32_t nz,
    uint32_t N)
{
    int counter = 0;

#pragma omp parallel for reduction(max \
                                   : N) num_threads(2)
    for (int i = 0; i < N; i++) {

        //fill arr1 with col index values
        int arr1_length = csc_col[i + 1] - csc_col[i];
        uint32_t* arr1 = (uint32_t*)malloc(arr1_length * sizeof(uint32_t));
        for (int k = csc_col[i], ii = 0; k < csc_col[i + 1]; k++, ii++) {
            arr1[ii] = csc_row[k];
        }

        for (int j = csc_col[i], k = csc_col[i + 1]; j < k; j++) {
            int row_new = csc_row[j];

            int arr2_length = csc_col[row_new + 1] - csc_col[row_new];
            uint32_t* arr2 = (uint32_t*)malloc(arr2_length * sizeof(uint32_t));
            for (int row_nest = csc_col[row_new], row_nest_up = csc_col[row_new + 1], iii = 0; row_nest < row_nest_up; row_nest++, iii++) {
                arr2[iii] = csc_row[row_nest];
            }
            //check if array has the same numbers
            for (int temp_i = 0; temp_i < arr1_length; temp_i++) {
                for (int temp_j = 0; temp_j < arr2_length; temp_j++) {
                    if (arr1[temp_i] == arr2[temp_j]) {
#pragma omp critical
                        counter++;
                        break;
                    }
                }
            }
            free(arr2);
        }
        free(arr1);
    }

    return counter;
}
