#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#ifndef NELEMS
#define NELEMS(a)       (sizeof(a) / sizeof((a)[0]))
#endif

static void swap(int arr[], int i, int j)
{
    int t;
    assert(arr);

    if (i == j)
        return;

    t = arr[i];
    arr[i] = arr[j];
    arr[j] = t;
    return;
}

static int bubble_sort(int arr[], size_t n)
{
    int i, j;

    if (!arr || n == 0)
        return -1;

    for (i = 0; i < n - 1; i++) {
        for (j = i; j < n; j++) {
            if (arr[i] < arr[j])
                swap(arr, i, j);
        }
    }

    return 0;
}

static int quick_sort(int arr[], size_t n)
{
    int i, j;

    if (!arr || n == 0)
        return -1;

    if (n == 1)
        return 0;

    i = rand() % n;
    swap(arr, 0, i);
    for (i = 1; i < n; i++) {
        if (arr[i] <= arr[0])
            continue;

        for (j = i + 1; j < n; j++) {
            if (arr[j] <= arr[0]) {
                swap(arr, i, j);
                break;
            }
        }
        if (j == n)
            break;
    }
    swap(arr, 0, i - 1);

    quick_sort(arr, i - 1);
    quick_sort(arr + i, n - i);
    return 0;
}

struct sort_algo {
    char *name;
    int (*fn)(int arr[], size_t n);
};

static struct sort_algo algorithms[] = {
    {"bsort", bubble_sort},
    {"qsort", quick_sort},
};

int main(int argc, char *argv[])
{
    int *arr, i, n;
    char *prog;

    /* preparation */
    srand(time(NULL));
    prog = strrchr(argv[0], '/');
    if (prog)
        prog++;
    else
        prog = argv[0];

    n = argc - 1;
    arr = malloc(n * sizeof(int));
    if (!arr)
        exit(1);
    for (i = 0; i < n; i++)
        arr[i] = atoi(argv[i + 1]);

    /* find algorithm first then sort */
    for (i = 0; i < NELEMS(algorithms); i++) {
        if (strcmp(prog, algorithms[i].name) == 0) {
            fprintf(stderr, "running %s ...\n", algorithms[i].name);
            if (algorithms[i].fn(arr, n) != 0)
                exit(1); 
            break;
        }
    }
    /* if no algorithm specified, use default one */
    if (i == NELEMS(algorithms)) {
        fprintf(stderr, "running default sort ...\n");
        if (quick_sort(arr, n) != 0)
            exit(1);
    }

    /* results */
    for (i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");

    free(arr);
    exit(0);
}
