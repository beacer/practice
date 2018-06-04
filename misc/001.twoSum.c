/*
 * https://leetcode.com/problemset/algorithms/
 *
 * Given an array of integers, find two numbers such that they add up to a specific target number.
 *
 * The function twoSum should return indices of the two numbers such that they add up to the target, where index1 must be less than index2. Please note that your returned answers (both index1 and index2) are not zero-based.
 *
 * You may assume that each input would have exactly one solution.
 *
 * Input: numbers={2, 7, 11, 15}, target=9
 * Output: index1=1, index2=2 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int *twoSum(int arr[], size_t n, int target)
{
    int i, j;
    int *indexes;

    if (!arr || n < 2)
        return NULL;

    for (i = 0; i < n - 1; i++) {
        for (j = i + 1; j < n; j++)
            if (arr[i] + arr[j] == target) {
                indexes = malloc(2 * sizeof(int));
                if (!indexes)
                    return NULL;

                indexes[0] = i;
                indexes[1] = j;
                return indexes;
            }
    }

    return NULL;
}

#define NEXT_ARG        argc--, argv++

int main(int argc, char *argv[])
{
    int target, *arr, i;
    int *indexes;

    if (argc < 2) {
        fprintf(stderr, "usage: ./a.out TARGET NUMBER NUMBER [NUMBER ...]\n");
        exit(1);
    }
    NEXT_ARG;

    target = atoi(argv[0]);
    NEXT_ARG;

    arr = malloc(argc * sizeof(int));
    if (!arr)
        exit(1);
    for (i = 0; i < argc; i++)
        arr[i] = atoi(argv[i]);

    indexes = twoSum(arr, argc, target);
    if (!indexes)
        fprintf(stderr, "fail to twoSum\n");
    else {
        fprintf(stderr, "indexes: %d %d\n", indexes[0], indexes[1]);
        free(indexes);
    }

    free(arr);
    exit(0);
}
