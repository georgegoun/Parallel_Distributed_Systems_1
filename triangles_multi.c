#include "coo2csc.h"
#include "mmio.h"

#include "timer.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#define NUMOFTHREADS 1000

#ifdef METHOD

#if METHOD == 1
#include "sequential_triangles_counting.h"

#elif METHOD == 2
#include "multithreading_triangles_counting.h"
#include <pthread.h>

#elif METHOD == 3
#include "opencilk_triangles_counting.h"
#endif

#elif METHOD == 4
#include "openmp_triangles_counting.h"
#endif

typedef struct Struct {
    uint32_t* csc_row;
    uint32_t* csc_col;
    uint32_t N;
    int* counter;
    int num_threads;
    int id_thread;
    pthread_mutex_t* mutex;
} makeStruct;

int main(int argc, char* argv[])
{
    FILE* file;
    MM_typecode matcode;
    int ret_code;
    int M, N, nz;
    int i, *I, *J;
    char* filepath;
    int method_code;
    pthread_t* threads;
    makeStruct* arguments;

    //timers
    struct timespec start_seq = { 0 }, stop_seq = { 0 };
    struct timespec start_pthreads = { 0 }, stop_pthreads = { 0 };
    struct timespec start_opencilk = { 0 }, stop_opencilk = { 0 };
    struct timespec start_openmp = { 0 }, stop_openmp = { 0 };

    //Open data

    if (argc == 1) {
        printf("Please enter all the essential arguments\nType: ./(program) path(data file) N(method code)\n");
        return (0);
    }
    if (argc == 2) {
        printf("Please enter all the essential arguments\nType: ./(program) path(data file) N(method code)\n");
        return (0);
    }
    if (argc == 3) {
        filepath = (argv[1]);
        method_code = atoi(argv[2]);
    }

    //com-Youtube, belgium_osm.mtx;
    if ((file = fopen(filepath, "r")) == NULL) {
        perror("Error in file open");
        exit(1);
    }

    if (mm_read_banner(file, &matcode) != 0) {
        perror("Can't proccess the file\n");
        exit(1);
    }

    if (mm_is_pattern(matcode) != 1) {
        printf("Please enter a matrix pattern");
        return (0);
    }

    /* find out size of sparse matrix .... */

    if ((ret_code = mm_read_mtx_crd_size(file, &M, &N, &nz)) != 0)
        exit(1);

    /* reseve memory for matrices */

    I = (int*)malloc(nz * sizeof(int));
    J = (int*)malloc(nz * sizeof(int));

    //switched I and J arguments of fscanf to generate upper triangular matrix
    for (i = 0; i < nz; i++) {
        fscanf(file, "%d %d\n", &J[i], &I[i]);
        I[i]--; /* adjust from 1-based to 0-based */
        J[i]--;
    }

    uint32_t* csc_row = (uint32_t*)malloc(nz * sizeof(uint32_t));
    uint32_t* csc_col = (uint32_t*)malloc((N + 1) * sizeof(uint32_t));

    coo2csc(csc_row, csc_col, (uint32_t*)I, (uint32_t*)J, nz, N, 1);

    switch (method_code) {

        /*              Sequential              */
#if METHOD == 1
    case 1:

        start_seq = timerStart(start_seq);

        int seq_triangles = seq_counting(csc_row, csc_col, nz, N);

        stop_seq = timerStop(stop_seq);

        printf("\nSequential %d triangles through %d columns in %lf seconds\n", seq_triangles, N, timeDif(start_seq, stop_seq));

        break;
#endif
        /*              Pthreads               */
#if METHOD == 2
    case 2:

        arguments = (makeStruct*)malloc(sizeof(makeStruct) * NUMOFTHREADS);

        int* count;
        count = malloc(sizeof(int));
        *count = 0;

        threads = (pthread_t*)malloc(NUMOFTHREADS * sizeof(*threads));

        pthread_mutex_t* mtx;
        mtx = malloc(sizeof(pthread_mutex_t));
        pthread_mutex_init(mtx, NULL);

        start_pthreads = timerStart(start_pthreads);

        for (int i = 0; i < NUMOFTHREADS; i++) {
            arguments[i].id_thread = i;
            arguments[i].csc_row = csc_row;
            arguments[i].csc_col = csc_col;
            arguments[i].N = N;
            arguments[i].num_threads = NUMOFTHREADS;
            arguments[i].counter = count;

            arguments[i].mutex = mtx;

            pthread_create(&threads[i], NULL, multi_counting, (void*)(&arguments[i]));
        }

        for (int i = 0; i < NUMOFTHREADS; i++) {
            pthread_join(threads[i], NULL);
        }
        stop_pthreads = timerStop(stop_pthreads);

        printf("\nMultithreading %d triangles in %lf seconds\n",
            (*(arguments->counter)), timeDif(start_pthreads, stop_pthreads));

        break;
#endif
        /*              OpenCilk            */

#if METHOD == 3
    case 3:
        start_opencilk = timerStart(start_opencilk);
        int opencilk_triangles = opencilk_counting(csc_row, csc_col, nz, N);
        stop_opencilk = timerStop(stop_opencilk);
        printf("\nOpenCilk %d triangles through %d columns in %lf seconds\n", opencilk_triangles, N, timeDif(start_opencilk, stop_opencilk));
        break;
#endif
        /*              OpenMP            */
#if METHOD == 4
    case 4:
        start_openmp = timerStart(start_openmp);
        int openmp_triangles = openmp_counting(csc_row, csc_col, nz, N);
        stop_openmp = timerStop(stop_openmp);
        printf("\nOpenMP %d triangles through %d columns in %lf seconds\n", openmp_triangles, N, timeDif(start_openmp, stop_openmp));
        break;
    default:
        printf("Please enter a valid method code\n");
#endif
    }

    free(csc_row);
    free(csc_col);

    return 0;
}