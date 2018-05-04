#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_NUMBERS	6

struct time_val {
	int hour;
	int min;
	int sec;
};

static int num_cmp(const void *a, const void *b)
{
	int x = *(int *)a;
	int y = *(int *)b;

	if (x > y)
		return 1;
	else if (x == y)
		return 0;
	else
		return -1;
}

/*
 * return the combined min two-numbers value or -1 on error.
 */
static int get_two_num_min(int numbers[MAX_NUMBERS])
{
	int i, j, res;

	for (i = 0; i < MAX_NUMBERS; i++) {
		if (numbers[i] < 0)
			continue;

		break;
	}
	if (i == MAX_NUMBERS)
		return -1; /* no number left */

	for (j = i + 1; j < MAX_NUMBERS; j++) {
		if (numbers[j] < 0)
			continue;

		break;
	}
	if (j == MAX_NUMBERS)
		return -1; /* no number left */

	res = numbers[i] * 10 + numbers[j];

	numbers[i] = -1;
	numbers[j] = -1;

	return res;
}

static int gen_timenum(int numbers[MAX_NUMBERS], struct time_val *tv)
{
	tv->hour = get_two_num_min(numbers);
	if (tv->hour < 0 || tv->hour >= 24)
		return -1;

	tv->min = get_two_num_min(numbers);
	if (tv->min < 0 || tv->min >= 60)
		return -1;

	tv->sec = get_two_num_min(numbers);
	if (tv->sec < 0 || tv->sec >= 60)
		return -1;

	return 0;
}

int main(int argc, char *argv[])
{
	int numbers[MAX_NUMBERS];
	struct time_val tv;
	int i;

	/* get nubmers with sanity check. */
	if (argc != 2) {
		fprintf(stderr, "pls input 6 numbers\n");
		exit(EXIT_FAILURE);
	}

	if (strlen(argv[1]) != MAX_NUMBERS) {
		fprintf(stderr, "need 6 numbers\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < MAX_NUMBERS; i++) {
		char *cp = &argv[1][i];

		if (!isdigit(*cp)) {
			fprintf(stderr, "need digital number\n");
			exit(EXIT_FAILURE);
		}

		numbers[i] = *cp - '0';
		if (numbers[i] < 0 || numbers[i] > 9) {
			fprintf(stderr, "bad number\n");
			exit(EXIT_FAILURE);
		}
	}

	/*
	 * sort the numbers for later use.
	 * use qsort_r(3) for thread safe env
	 */
	qsort(numbers, MAX_NUMBERS, sizeof(int), num_cmp);

	if (gen_timenum(numbers, &tv) != 0)
		fprintf(stderr, "fail to generate time\n");
	else
		printf("%02d:%02d:%02d\n", tv.hour, tv.min, tv.sec);
}
