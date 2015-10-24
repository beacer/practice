#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

static void swap(int *a, int *b)
{
    int t;
    assert(a && b);

    t = *a;
    *a = *b;
    *b = t;
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
                swap(&arr[i], &arr[j]);
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int *arr, i, n;

    n = argc - 1;
    arr = malloc(n * sizeof(int));
    if (!arr)
        exit(1);
    for (i = 0; i < n; i++)
        arr[i] = atoi(argv[i + 1]);

    if (bubble_sort(arr, n) != 0)
        exit(1);

    for (i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");

    free(arr);
    exit(0);
}
