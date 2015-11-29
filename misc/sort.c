#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include "list.h"

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
    int i, l;

    if (!arr)
        return -1;
    if (n <= 1)
        return 0;

    swap(arr, 0, rand() % n);
    l = 0;
    for (i = l + 1; i < n; i++) {
        if (arr[i] <= arr[0])
            swap(arr, ++l, i);
    }
    swap(arr, 0, l);

    quick_sort(arr, l);
    quick_sort(arr + l + 1, n - l - 1);
}


/*
 * 'radix sort' (or 'card sort') is better solution for bucket sort.
 * it's like a multi-round 'bucket sort' to save the size of buckets.
 */
#define RADIX        10  /* other values is also ok */

static struct list_head *
rsort_hash(struct list_head bucket[], int num, int radix, int expo)
{
    int hash, base;

    base = (int)pow(radix, expo);
    hash = num / base % radix;
    return &bucket[hash];
}

static int radix_sort(int arr[], size_t n)
{
    struct elem {
        struct list_head list;
        int num;
    } *elem, *tmp;
    struct list_head bucket[RADIX], *head;
    struct list_head result; /* need it for temporary result :-) */
    int expo = 0, err = -1, i, max = -1, max_expo;
    assert(arr && n > 0);

    for (i = 0; i < RADIX; i++)
        INIT_LIST_HEAD(&bucket[i]);
    INIT_LIST_HEAD(&result);

    /* init the 'result' list */
    for (i = 0; i < n; i++) {
        elem = malloc(sizeof(struct elem));
        if (!elem)
            goto cleanup;

        max = max > arr[i] ? max : arr[i];

        INIT_LIST_HEAD(&elem->list);
        elem->num = arr[i];
        list_add_tail(&elem->list, &result);
    }
    for (max_expo = 0; max > 0; max /= RADIX)
        max_expo++;

    printf("tmp result: ");
    list_for_each_entry(elem, &result, list)
        printf("%d ", elem->num);
    printf("\n");

    /* if there's no elem equal-or-great than radix^expo,
     * then sort finished, or need next round */
    for (expo = 0; expo < max_expo; expo++) {
        /* for all elems in temporary result list  */
        list_for_each_entry_safe(elem, tmp, &result, list) {

            list_del(&elem->list);
            head = rsort_hash(bucket, elem->num, RADIX, expo);
            list_add_tail(&elem->list, head);
        }

        for (i = 0; i < RADIX; i++) {
            list_for_each_entry_safe(elem, tmp, &bucket[i], list) {
                list_del(&elem->list);
                list_add_tail(&elem->list, &result);
            }
        }

        for (i = 0; i < RADIX; i++)
            INIT_LIST_HEAD(&bucket[i]);

        printf("tmp result: ");
        list_for_each_entry(elem, &result, list)
            printf("%d ", elem->num);
        printf("\n");

    }

    i = 0;
    list_for_each_entry(elem, &result, list) {
        if (i >= n)
            break;
        arr[i++] = elem->num;
    }
    if (i != n)
        return -1;

    err = 0; /* success */

cleanup:
    // TODO: free all elems in bucket
    return err;
}

struct sort_alg {
    char *name;
    char *opt;
    int (*fn)(int arr[], size_t n);
};

static struct sort_alg algorithms[] = {
    {"quick", "-q", quick_sort},
    {"bubble", "-b", bubble_sort},
    {"radix", "-r", radix_sort},
};

static void usage(void)
{
    int i;

    printf("Usage: ./sort [OPTION] NUMBER [NUMBER ...]\n");
    printf("Options:\n");
    for (i = 0; i < NELEMS(algorithms); i++) {
        printf("    %s    %s sort\n", algorithms[i].opt, algorithms[i].name);
    }

    return;
}

#define NEXT_ARG     argc--, argv++

int main(int argc, char *argv[])
{
    int *arr, i, n;
    struct sort_alg *alg;

    /* select algorithm by option */
    NEXT_ARG;
    if (argc <= 0) {
        usage();
        exit(1);
    }

    alg = &algorithms[0];
    for (i = 0; i < NELEMS(algorithms); i++) {
        if (strcmp(argv[0], algorithms[i].opt) == 0) {
            alg = &algorithms[i];
            NEXT_ARG;
            break;
        }
    }

    /* preparation */
    srand(time(NULL));

    n = argc;
    arr = malloc(n * sizeof(int));
    if (!arr)
        exit(1);
    for (i = 0; i < n; i++)
        arr[i] = atoi(argv[i]);

    /* find algorithm first then sort */
    printf("runing %s sort ...\n", alg->name);
    if (alg->fn(arr, n) != 0) {
        printf("failed\n");
        exit(1);
    }

    /* results */
    for (i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");

    free(arr);
    exit(0);
}
