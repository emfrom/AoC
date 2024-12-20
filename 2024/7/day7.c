#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <stdbool.h>
#include "../utility_blob.c"

/*
  Notes day 7

  Parse numbers from the end, recursively test options (2/3-ary tree).
  - Prune impossible solutions early.
  - Leave function layout up to compiler optimizations.

  Worst case O(n^3)
  
  P2 runtime ~2ms
*/

#define PROBLEM_2

void print_intlist(int64_t *list, int list_end)
{
    for (int i = 0; i < list_end; i++)
        printf(" %lu", list[i]);

    return;
}

int64_t check_concat(int64_t leftvalue, int64_t number)
{
    int64_t remainder = -1;

    while (number && leftvalue % 10 == number % 10) {
        leftvalue = leftvalue / 10;
        number = number / 10;
        remainder = leftvalue;
    }

    return remainder;
}

int check_list(int64_t leftvalue, int64_t *list, int list_end)
{
    if (leftvalue < 0)          // addition and concat
        return false;

#if 0
    printf("%ld(%d):", leftvalue, list_end);
    print_intlist(list, list_end);
    printf("\n");
#endif

    if (list_end == 1)
        return list[0] == leftvalue;

    list_end -= 1;

    if (check_list(leftvalue - list[list_end], list, list_end))
        return true;

#ifdef PROBLEM_2
    if (check_list
        (check_concat(leftvalue, list[list_end]), list, list_end))
        return true;
#endif

    if (leftvalue % list[list_end])
        return false;

    return check_list(leftvalue / list[list_end], list, list_end);
}

int main()
{
    int n_lines;
    char **input = xload_lines("input", &n_lines);

#if 0
    printf("Loaded %u lines\n", n_lines);
#endif

    int64_t list_of_numbers[100];
    int64_t sum_of_valids = 0;

    for (int i = 0; i < n_lines; i++) {
        int num_numbers = 0;

        //Scan line
        char *temp = input[i];
        while (NULL != temp) {
            if (!sscanf(temp, "%ld", list_of_numbers + num_numbers)) {
                fprintf(stderr, "main() -> input\n");
                exit(EXIT_FAILURE);
            }

            num_numbers++;

            temp = strchr(temp, ' ');
            if (NULL != temp)
                temp++;
        }
        //check
        if (check_list
            (list_of_numbers[0], list_of_numbers + 1, num_numbers - 1)) {
            //add or whatever
            sum_of_valids += list_of_numbers[0];
        }
    }

#ifndef PROBLEM_2
    printf("Problem 1: %ld\n", sum_of_valids);
#else
    printf("Problem 2: %ld\n", sum_of_valids);
#endif

    xfree(input);

    return EXIT_SUCCESS;
}
