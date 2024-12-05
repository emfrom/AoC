#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../utility_blob.c"

// There are less than 128 actual pages => uint8_t
// This also means that each page can at most be before or after 128 entries
// Using two sorted arrays per entry is enough (and fast)
//
// Dirty trick by AoC, the real input was symmetric but test was not

#define NUMBER_MAX 100

// Logic of var names
// after_xx has to be after before_xxx
typedef struct condition_s {
    int n_after;
    uint8_t *after;

    int n_before;
    uint8_t *before;
} *condition;

struct condition_s condition_store[NUMBER_MAX];
uint8_t after_store[NUMBER_MAX * NUMBER_MAX];
uint8_t before_store[NUMBER_MAX * NUMBER_MAX];

condition condition_init(uint8_t num)
{
    condition cond = condition_store + num;

    cond->n_after = 0;
    cond->after = after_store + num * NUMBER_MAX;

    cond->n_before = 0;
    cond->before = before_store + num * NUMBER_MAX;

    return cond;
}

#if 0
void condition_print(uint8_t num)
{
    condition c = condition_store + num;

    if (0 == c->n_after)
        return;

    printf("Page: %u\n", num);

    printf("B %d:", c->n_before);
    for (int i = 0; i < c->n_before; i++)
        printf(" %u", c->before[i]);

    printf("\nA %d:", c->n_after);
    for (int i = 0; i < c->n_after; i++)
        printf(" %u", c->after[i]);

    printf("\n");
}
#endif

//For inserting and finding
int binary_search(uint8_t *array, uint8_t item, int low, int high)
{
    while (low < high) {
        int mid = low + (high - low) / 2;

        if (item == array[mid])
            return mid;

        if (item < array[mid])
            high = mid;
        else
            low = mid + 1;
    }
    return low;
}


int insert_sorted(uint8_t *array, int n_array, uint8_t toinsert)
{

    int pos;
    pos = binary_search(array, toinsert, 0, n_array);

    //Overlaps
    for (int i = n_array; i > pos; i--)
        array[i] = array[i - 1];

    array[pos] = toinsert;

    return n_array + 1;
}

bool condition_check(uint8_t is_before, uint8_t is_after)
{
    condition cond;

    // If is_before is in (have to be) after list, false
    cond = condition_store + is_after;
    for (int i = 0; i < cond->n_after; i++) {
        int index =
            binary_search(cond->after, is_before, 0, cond->n_after);

        if (cond->after[index] == is_before)
            return false;
    }


    // If is_after is in (have to be) before list, false
    cond = condition_store + is_before;
    for (int i = 0; i < cond->n_before; i++) {
        int index =
            binary_search(cond->before, is_after, 0, cond->n_before);

        if (cond->before[index] == is_after)
            return false;
    }

    return true;
}


bool check_pages(int n_ints, int *sequence)
{
    bool check_ok = true;

    for (int i = 1; check_ok && i < n_ints; i++)
        for (int j = 0; check_ok && j < i; j++)
            check_ok = condition_check(sequence[j], sequence[i]);

    return check_ok;
}


int page_compare(const void *before, const void *after)
{
    int b = *((int *) before);
    int a = *((int *) after);

    //seeking middle, direction doesnt matter
    if (condition_check(b, a))
        return -1;

    return 1;
}


void fix_pages(int n_pages, int *pages)
{
    qsort(pages, n_pages, sizeof(int), page_compare);
}


condition add_rule(uint8_t new_before, uint8_t new_after)
{
    condition temp;
    temp = condition_store + new_before;
    temp->n_after = insert_sorted(temp->after, temp->n_after, new_after);

    temp = condition_store + new_after;
    temp->n_before =
        insert_sorted(temp->before, temp->n_before, new_before);

    return temp;
}


int main()
{

    // Init conditions
    for (uint8_t i = 0; i < NUMBER_MAX; i++)
        condition_init(i);

    char **input_lines = xload_lines("input", NULL);

    // Populate conditions
    int current_line = 0;
    int before, after;

    while (2 ==
           sscanf(input_lines[current_line], "%d|%d", &before, &after)) {
        add_rule(before, after);
        current_line++;
    }

#if 0
    for (int i = 0; i < NUMBER_MAX; i++)
        condition_print(i);
#endif

    // Check conditions
    long middle_number_sum_p1 = 0;
    long middle_number_sum_p2 = 0;
    int pages[NUMBER_MAX];
    char *temp;
    while (NULL != (temp = input_lines[current_line])) {

        int n_pages = 0;

        for (;;) {
            if (1 != sscanf(temp, "%d", pages + n_pages)) {
                fprintf(stderr, "main() -> Bad input\n");
                exit(EXIT_FAILURE);
            }
            n_pages++;

            temp = strchr(temp, ',');
            if (NULL == temp)
                break;
            temp++;
        }

        if (check_pages(n_pages, pages)) {
            middle_number_sum_p1 += pages[n_pages / 2];
#if 0
            printf("%s\n", input_lines[current_line]);
#endif
        } else {                // Bad pages
            fix_pages(n_pages, pages);
            middle_number_sum_p2 += pages[n_pages / 2];
        }

        current_line++;
    }

    printf("Problem 1: %ld\n", middle_number_sum_p1);
    printf("Problem 2: %ld\n", middle_number_sum_p2);



    return EXIT_SUCCESS;        //free everything :)
}
