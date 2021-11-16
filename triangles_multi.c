#include "coo2csc.h"
#include "mmio.h"
#include "multithreading_triangles_counting.h"
#include "sequential_triangles_counting.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#define NUMOFTHREADS 3

typedef struct Struct {
    uint32_t* csc_row;
    uint32_t* csc_col;
    uint32_t nz;
    uint32_t N;
    int counter;
    int num_threads;
    int id_thread;
    pthread_mutex_t lock;
} makeStruct;

int main(int argc, char* argv[])
{
    FILE* file;
    MM_typecode matcode;
    int ret_code;
    int M, N, nz;
    int i, *I, *J;
    pthread_t* threads;
    pthread_attr_t pthread_custom_attr;
    time_t start_multi_pthreads, end_multi_pthreads;

    if ((file = fopen("../belgium_osm.mtx", "r")) == NULL) {
        perror("Error in file open");
        exit(1);
    }

    if (mm_read_banner(file, &matcode) != 0) {
        perror("Can't proccess the file\n");
        exit(1);
    }

    printf(" %d\n", mm_is_pattern(matcode));

    /* find out size of sparse matrix .... */

    if ((ret_code = mm_read_mtx_crd_size(file, &M, &N, &nz)) != 0)
        exit(1);

    /* reseve memory for matrices */

    I = (int*)malloc(nz * sizeof(int));
    J = (int*)malloc(nz * sizeof(int));

    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
    /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */

    //switched I and J arguments of fscanf to generate upper triangular matrix
    for (i = 0; i < nz; i++) {
        fscanf(file, "%d %d\n", &J[i], &I[i]);
        I[i]--; /* adjust from 1-based to 0-based */
        J[i]--;
    }

    uint32_t* csc_row = (uint32_t*)malloc(nz * sizeof(uint32_t));
    uint32_t* csc_col = (uint32_t*)malloc((N + 1) * sizeof(uint32_t));

    coo2csc(csc_row, csc_col, (uint32_t*)I, (uint32_t*)J, nz, N, 1);

    //multithreading

    makeStruct* arguments;
    arguments = (makeStruct*)malloc(sizeof(makeStruct) * NUMOFTHREADS);

    arguments->csc_row = csc_row;
    arguments->csc_col = csc_col;
    arguments->nz = nz;
    arguments->N = N;
    arguments->counter = 0;
    arguments->num_threads = NUMOFTHREADS;

    pthread_mutex_t locked = PTHREAD_MUTEX_INITIALIZER;
    arguments->lock = locked;

    start_multi_pthreads = clock();
    threads = (pthread_t*)malloc(NUMOFTHREADS * sizeof(*threads));
    pthread_attr_init(&pthread_custom_attr);
    for (int i = 0; i < NUMOFTHREADS; i++) {
        arguments->id_thread = i;
        pthread_create(&threads[i], NULL, multi_counting, arguments);
    }

    for (int i = 0; i < NUMOFTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    end_multi_pthreads = clock();

    printf("to count %d triangles through %d columns\n", seq_counting(csc_row, csc_col, nz, N), N);
    printf("Multi threading %d in %ld seconds\n",
        arguments->counter, (end_multi_pthreads - start_multi_pthreads) / CLOCKS_PER_SEC);
    free(csc_row);
    free(csc_col);

    return 0;
}

// void* multis_counting(void* args)
// {

//     makeStruct* arguments = (makeStruct*)args;

//     //typedef arguments->csc_row csc_row;

//     //int counter = 0;

//     //simple calculation

//     //uint32_t csc_row[] = { 1, 4, 0, 4, 3, 2, 0, 1 };
//     //uint32_t csc_col[] = { 0, 2, 4, 5, 6, 8 };

//     for (int i = 0; i < arguments->N; i++) {

//         //fill arr1 with col index values
//         int arr1_length = arguments->csc_col[i + 1] - arguments->csc_col[i];
//         uint32_t* arr1 = (uint32_t*)malloc(arr1_length * sizeof(uint32_t));
//         for (int k = arguments->csc_col[i], ii = 0; k < arguments->csc_col[i + 1]; k++, ii++) {
//             arr1[ii] = arguments->csc_row[k];
//         }

//         for (int j = arguments->csc_col[i], k = arguments->csc_col[i + 1]; j < k; j++) {
//             int row_new = arguments->csc_row[j];

//             int arr2_length = arguments->csc_col[row_new + 1] - arguments->csc_col[row_new];
//             uint32_t* arr2 = (uint32_t*)malloc(arr2_length * sizeof(uint32_t));
//             for (int row_nest = arguments->csc_col[row_new], row_nest_up = arguments->csc_col[row_new + 1], iii = 0; row_nest < row_nest_up; row_nest++, iii++) {
//                 arr2[iii] = arguments->csc_row[row_nest];
//             }

//             //check if array has the same numbers
//             for (int temp_i = 0; temp_i < arr1_length; temp_i++) {
//                 for (int temp_j = 0; temp_j < arr2_length; temp_j++) {
//                     if (arr1[temp_i] == arr2[temp_j]) {
//                         arguments->counter++;
//                         break;
//                     }
//                 }
//             }
//             free(arr2);
//         }
//         free(arr1);
//         //printf("col: %d\n", i);
//     }
//     return 0;
// }