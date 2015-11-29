#include <stdio.h>
#include <stdlib.h>

int bin_search(int arr[], int start, int end, int elem)
{
    int pos;

    if (!arr || start == end)
        return -1;

    pos = (end - start) / 2 + start;
    if (arr[pos] == elem)
        return pos;
    else if (arr[pos] < elem)
        return bin_search(arr, pos + 1, end, elem);
    else
        return bin_search(arr, start, pos, elem);
}

#define NEXT_ARG     argc--, argv++

int main(int argc, char *argv[])
{
    int *arr, i, n, idx, elem;

    NEXT_ARG;
    if (argc <= 0) {
        fprintf(stderr, "./a.out TARGET NUMBER [NUMBER ...]\n");
        exit(1);
    }

    elem = atoi(argv[0]);
    NEXT_ARG;

    n = argc;
    arr = malloc(n * sizeof(int));
    if (!arr)
        exit(1);
    for (i = 0; i < n; i++)
        arr[i] = atoi(argv[i]);

    idx = bin_search(arr, 0, n, elem);
    if (idx >= 0 && idx < n)
        printf("Elem %d is at position %d\n", elem, idx);
    else
        printf("Elem %d not found\n", elem);

    exit(0);
}

