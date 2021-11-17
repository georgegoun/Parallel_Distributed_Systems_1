#include "multithreading_triangles_counting.h"
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Struct {
    uint32_t* csc_row;
    uint32_t* csc_col;
    uint32_t nz;
    uint32_t N;
    int* counter;
    int num_threads;
    int id_thread;
    pthread_mutex_t mutex;
    pthread_cond_t modif;

} makeStruct;

void run(int id, uint32_t N, int num_threads)
{
    printf("Thread inside function: %d\n", id);
    for (int i = id; i < N; i += num_threads) {
        //if (id == 0) {
        //    printf("0 inside loop function\n");
        //}
        if (id == 1) {
            printf("1 inside loop function\n");
        }
        if (id == 2) {
            printf("2 inside loop function\n");
        }
    }
}
void* multi_counting(void* args)
{
    makeStruct* arg = (makeStruct*)args;

    //pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

    //tried stepping up values but too slow
    //printf("thread inside func: %d\n", arg->id_thread);

    // for (int i = 0, gaga = 0, baba = 0; i < 30; i++) {
    //     if (arg->id_thread == 1) {
    //         printf("thread 1 executed %d tim\n", gaga);
    //         gaga++;
    //     }
    //     if (arg->id_thread == 2) {
    //         printf("thread 2 executed %d tim\n", baba);
    //         baba++;
    //     }
    // }
    //int* zara = NULL;
    //zara = malloc(sizeof(int) * arg->num_threads);

    //run(arg->id_thread, arg->N, arg->num_threads);

    for (int i = arg[arg->id_thread].id_thread; i < arg->N; i += arg->num_threads) {

        //while (arg->id_thread != 2) {
        if (arg->id_thread == 1) {
            printf("thread executed ok\n");
        }
        //seperated i in numofthreads
        //for (int i = ((arg->id_thread) * (arg->N / arg->num_threads)); i < ((arg->id_thread == arg->num_threads - 1) ? (arg->N) : ((arg->id_thread + 1) * (arg->N / (arg->num_threads)))); i++) {
        //fill arr1 with col index values
        int arr1_length = arg->csc_col[i + 1] - arg->csc_col[i];
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
                        //pthread_mutex_lock(&(arg->mutex));
                        (*(arg->counter))++;
                        //printf("Thread %d running and counter is %d\n", (arg->id_thread), (*(arg->counter)));
                        //pthread_mutex_unlock(&(arg->mutex));
                        break;
                    }
                }
            }

            free(arr2);
        }
        free(arr1);
        //printf("col: %d\n", i);
    }
    printf("threadddd %d\n", arg->id_thread);
    pthread_exit(NULL);
}
