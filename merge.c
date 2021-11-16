typedef struct MergreStruct {
    int num_threads;
    int N;
    unint32_t* path;
    int id_thread;
} makeMergeStruct;

void merge(int arr[], int l, int m, int r);
//void mergeSort(int arr[], int l, int r);
void* mergeSort(void* arg);
/* l is for left index and r is right index of the
sub-array of arr to be sorted */
//void mergeSort(int arr[], int l, int r);
//void compare_mergeSorted {};

void main()
{
    //for fun: compare usind Pthreads and Merge Sort

    //https://www.geeksforgeeks.org/merge-sort-using-multi-threading/

    //https://stackoverflow.com/questions/40937105/how-can-i-concatenate-two-arrays-in-c-using-memcpy

    // int* merge_path = (unit32_t*)malloc((arr1_length + arr2_length) * sizeof(uint32_t));

    // memccpy(merge_path, arr1, arr1_length);
    // memccpy(merge_path, arr2, arr2_length);

    //maybe check with merge sort parellelism:
    //merging both arrs till n/(num_threads*2)(allways odd)
    //then i have num_threads boths to check
    //https://stackoverflow.com/questions/17184553/detecting-duplicates-using-divide-and-conquer-merge-sort

    // makeMergeStruct* merge_args;
    // merge_args = (makeMergeStruct*)malloc(sizeof(makeMergeStruct) * arg->num_threads);
    // typedef struct Mergre_Struct {
    //     int num_threads;
    //     int N;
    //     unint32_t* path;
    //     int id_thread;
    // } makeMergeStruct;
    // merge_args->num_threads = arg->num_threads;
    // merge_args->N = arg->N;
    // merge_args->path = merge_path;
    // for (int i = 0; i < arg->num_threads; i++) {
    //     merge_args->id_thread = i;
    //     pthread_create(&threads[i], NULL, mergeSort, merge_args);
    // }

    // for (int i = 0; i < NUMOFTHREADS; i++) {
    //     pthread_join(threads[i], NULL);
    // }
    ////mergeSort(path, 0, arr1_length + arr2_length - 1);
}
/* l is for left index and r is right index of the
sub-array of arr to be sorted */

// void mergeSort(int arr[], int l, int r)

void* mergeSort(void* args)

{

    makeMergeStruct* merge_nested_args = (makeMergeStruct*)args;
    // merge_nested_args->id_thread
    //     merge_nested_args->num_threads
    //         merge_nested_args->path
    //             merge_nested_args->N
    //                 merge_nested_args->thread_id
    int thread_part = (merge_nested_args->id_thread)++;

    int low = thread_part * (N / merge_nested_args->num_threads);
    int high = (thread_part + 1) * (N / (merge_nested_args->num_threads) - 1);
    if (l < r) {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l + (r - l) / 2;

        // Sort first and second halves
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);

        // Compare first and second halves

        for (int i = 0; i < m; i++) {
            for (int j = m; m++) {
                if (arr[i] == arr[j]) {
                    arg->counter++;
                    break;
                }
            }
        }
        // for (int temp_i = 0; temp_i < arr1_length; temp_i++) {
        //     for (int temp_j = 0; temp_j < arr2_length; temp_j++) {
        //         if (arr1[temp_i] == arr2[temp_j]) {
        //             arg->counter++;
        //             break;
        //         }
        //     }
        // }

        merge(arr, l, m, r);
    }
}

void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    /* create temp arrays */
    int L[n1], R[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
    are any */
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
    are any */
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}