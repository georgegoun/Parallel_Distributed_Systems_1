#include "multithreading_triangles_counting.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Struct {
    uint32_t* csc_row;
    uint32_t* csc_col;
    uint32_t nz;
    uint32_t N;
    int counter;
    int num_threads;
    int id_thread;
} makeStruct;

void* multi_counting(void* args)
{

    // typedef struct Struct {
    //     uint32_t* csc_row;
    //     uint32_t* csc_col;
    //     uint32_t nz;
    //     uint32_t N;
    //     int counter;
    // } makeStruct;

    makeStruct* arguments = (makeStruct*)args;

    //typedef arguments->csc_row csc_row;

    //int counter = 0;

    //simple calculation

    //uint32_t csc_row[] = { 1, 4, 0, 4, 3, 2, 0, 1 };
    //uint32_t csc_col[] = { 0, 2, 4, 5, 6, 8 };

    for (int i = 0; i < arguments->N; i++) {

        //fill arr1 with col index values
        int arr1_length = arguments->csc_col[i + 1] - arguments->csc_col[i];
        uint32_t* arr1 = (uint32_t*)malloc(arr1_length * sizeof(uint32_t));
        for (int k = arguments->csc_col[i], ii = 0; k < arguments->csc_col[i + 1]; k++, ii++) {
            arr1[ii] = arguments->csc_row[k];
        }

        for (int j = arguments->csc_col[i], k = arguments->csc_col[i + 1]; j < k; j++) {
            int row_new = arguments->csc_row[j];

            int arr2_length = arguments->csc_col[row_new + 1] - arguments->csc_col[row_new];
            uint32_t* arr2 = (uint32_t*)malloc(arr2_length * sizeof(uint32_t));
            for (int row_nest = arguments->csc_col[row_new], row_nest_up = arguments->csc_col[row_new + 1], iii = 0; row_nest < row_nest_up; row_nest++, iii++) {
                arr2[iii] = arguments->csc_row[row_nest];
            }

            //check if array has the same numbers
            for (int temp_i = 0; temp_i < arr1_length; temp_i++) {
                for (int temp_j = 0; temp_j < arr2_length; temp_j++) {
                    if (arr1[temp_i] == arr2[temp_j]) {
                        arguments->counter++;
                        break;
                    }
                }
            }
            free(arr2);
        }
        free(arr1);
        //printf("col: %d\n", i);
    }
    return 0;
}