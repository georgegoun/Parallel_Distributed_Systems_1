#include "multithreading_triangles_counting.h"
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Struct {
    uint32_t* csc_row;
    uint32_t* csc_col;
    uint32_t N;
    int* counter;
    int num_threads;
    int id_thread;
    pthread_mutex_t* mutex;

} makeStruct;

void* multi_counting(void* args)
{

    makeStruct* arg = (makeStruct*)args;

    //pthread_mutex_lock(&(arg->mutex));

    //tried stepping up num_thread++ steps but too slow
    //for (int i = arg->id_thread; i < arg->N; i += arg->num_threads) {

    //printf("ThreadID %d: Just started i %d for thread %d\n", (int)pthread_self(), i, arg->id_thread);

    //seperated i in numofthreads
    pthread_mutex_lock((arg->mutex));

    int i = ((arg->id_thread) * (arg->N / arg->num_threads));
    int max = ((arg->id_thread == arg->num_threads - 1) ? (arg->N) : ((arg->id_thread + 1) * (arg->N / (arg->num_threads))));

    for (; i < max; i++) {
        //fill arr1 with col index values
        //printf("ThreadID %d for i: %d\n", arg->id_thread, i);

        int arr1_length = arg->csc_col[i + 1] - arg->csc_col[i];
        //int arr1[arr1_length];
        uint32_t* arr1 = (uint32_t*)malloc(arr1_length * sizeof(uint32_t));
        for (int k = arg->csc_col[i], ii = 0; k < arg->csc_col[i + 1]; k++, ii++) {
            arr1[ii] = arg->csc_row[k];
        }

        for (int j = arg->csc_col[i], k = arg->csc_col[i + 1]; j < k; j++) {
            int row_new = arg->csc_row[j];

            int arr2_length = arg->csc_col[row_new + 1] - arg->csc_col[row_new];
            uint32_t* arr2 = (uint32_t*)malloc(arr2_length * sizeof(uint32_t));
            for (int row_nest = arg->csc_col[row_new], row_nest_up = arg->csc_col[row_new + 1], iii = 0; row_nest < row_nest_up; row_nest++, iii++) {
                arr2[iii] = arg->csc_row[row_nest];
            }

            //important

            for (int temp_i = 0; temp_i < arr1_length; temp_i++) {
                for (int temp_j = 0; temp_j < arr2_length; temp_j++) {
                    if (arr1[temp_i] == arr2[temp_j]) {

                        //pthread_mutex_lock((arg->mutex));
                        (*(arg->counter))++;
                        //printf("Thread %d running and counter is %d\n", arg->id_thread, (*(arg->counter)));

                        //pthread_mutex_unlock((arg->mutex));
                        break;
                    }
                }
            }

            free(arr2);
        }
        free(arr1);
    }
    pthread_mutex_unlock((arg->mutex));

    pthread_exit(NULL);
}
