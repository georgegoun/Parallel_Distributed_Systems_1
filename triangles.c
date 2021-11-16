#include "coo2csc.h"
#include "mmio.h"
#include "sequential_triangles_counting.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int sequential_counting(uint32_t* csc_row,
    uint32_t* csc_col,
    uint32_t nz,
    uint32_t N);

int main(int argc, char* argv[])
{
    FILE* file;
    MM_typecode matcode;
    int ret_code;
    int M, N, nz;
    int i, *I, *J;

    if ((file = fopen("belgium_osm.mtx", "r")) == NULL) {
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

    printf("to count %d triangles through %d columns\n", sequential_counting(csc_row, csc_col, nz, N), N);

    free(csc_row);
    free(csc_col);

    return 0;
}

int multi_counting(uint32_t* csc_row,
    uint32_t* csc_col,
    uint32_t nz,
    uint32_t N)
{
    return 0;
};

int sequential_counting(uint32_t* csc_row,
    uint32_t* csc_col,
    uint32_t nz,
    uint32_t N)
{
    time_t start, end;
    int counter = 0;

    //simple calculation

    //uint32_t csc_row[] = { 1, 4, 0, 4, 3, 2, 0, 1 };
    //uint32_t csc_col[] = { 0, 2, 4, 5, 6, 8 };

    start = clock();
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
                        counter++;
                        break;
                    }
                }
            }
            free(arr2);
        }
        free(arr1);
        //printf("col: %d\n", i);
    }
    end = clock();
    printf("Single processor time: %ld, ", (end - start) / CLOCKS_PER_SEC);
    return counter;
}