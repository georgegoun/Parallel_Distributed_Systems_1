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
#define NUMOFTHREADS 100

typedef struct Struct {
    uint32_t* csc_row;
    uint32_t* csc_col;
    uint32_t nz;
    uint32_t N;
    int* counter;
    int num_threads;
    int id_thread;
    pthread_mutex_t* mutex;
    pthread_cond_t modif;
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
    clock_t start_multi_pthreads, end_multi_pthreads;
    //com-Youtube, belgium_osm.mtx;
    if ((file = fopen("../com-Youtube.mtx", "r")) == NULL) {
        perror("Error in file open");
        exit(1);
    }

    if (mm_read_banner(file, &matcode) != 0) {
        perror("Can't proccess the file\n");
        exit(1);
    }

    // printf(" %d\n", mm_is_pattern(matcode));

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
    int seq_triangles = seq_counting(csc_row, csc_col, nz, N);
    printf("to count %d triangles through %d columns\n", seq_triangles, N);

    //multithreading

    makeStruct* arguments;

    arguments = (makeStruct*)malloc(sizeof(makeStruct) * NUMOFTHREADS);

    int* count;
    count = malloc(sizeof(int));
    *count = 0;

    //pthread_mutex_init(&(arguments->mutex), NULL);

    pthread_cond_t modified = PTHREAD_COND_INITIALIZER;
    arguments->modif = modified;
    // if (pthread_mutex_init(&arguments->lock, NULL) != 0) {
    //     printf("\n mutex init has failed\n");
    //     return 1;
    // }

    start_multi_pthreads = clock();
    threads = (pthread_t*)malloc(NUMOFTHREADS * sizeof(*threads));
    pthread_attr_init(&pthread_custom_attr);

    pthread_mutex_t* mtx;
    mtx = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mtx, NULL);

    for (int i = 0; i < NUMOFTHREADS; i++) {
        arguments[i].id_thread = i;
        arguments[i].csc_row = csc_row;
        arguments[i].csc_col = csc_col;
        arguments[i].nz = nz;
        arguments[i].N = N;
        arguments[i].num_threads = NUMOFTHREADS;
        arguments[i].counter = count;

        arguments[i].mutex = mtx;
        //wrong initialize
        //pthread_mutex_init((arguments[i].mutex), NULL);
        // printf("Mutex: %d for i %d\n", (int)(arguments[i].mutex), i);
        pthread_create(&threads[i], NULL, multi_counting, (void*)(&arguments[i]));
    }

    for (int i = 0; i < NUMOFTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    end_multi_pthreads = clock();

    printf("\nMultithreading %d in %f seconds\n",
        (*(arguments->counter)), ((double)(end_multi_pthreads - start_multi_pthreads) / CLOCKS_PER_SEC));
    free(csc_row);
    free(csc_col);

    return 0;
}
