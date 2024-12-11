#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "../utility_blob.c"


uint64_t *make_uint64(uint64_t input)
{
    uint64_t *space = xmalloc(sizeof(uint64_t));

    *space = input;

    return space;
}



void print_list(void *data, void *supplemental)
{
    FILE *fp = supplemental;
    uint64_t *num = data;
    fprintf(fp, "%lu ", *num);
}



int num_digits(uint64_t number)
{
    int digits = 1;

    while (0 < (number = number / 10))
        digits++;

    return digits;
}

// u64 has at most 20 digits base 10
// half, cause we are splitting numbers
const uint64_t modulus_map[11] = { 1, 10, 100, 1000,
    10000, 100000, 1000000, 10000000,
    100000000, 1000000000, 10000000000
};


linked_list blink(linked_list list, uint64_t *num_stones)
{

    if (NULL == list)
        return NULL;

    uint64_t *number = list->data;

    if (*number == 0) {
        *number = 1;
        blink(list->next, num_stones);
        return list;
    }

    int n_digits = num_digits(*number);

    if (n_digits % 2) {
        *number *= 2024;
        blink(list->next, num_stones);
        return list;
    }
    //Split the number
    uint64_t right = *number % modulus_map[n_digits >> 1];
    uint64_t left = *number / modulus_map[n_digits >> 1];
    *num_stones += 1;

    list->next = linked_list_prepend(list->next, make_uint64(right));   //Preserve node order
    *number = left;

    blink(list->next->next, num_stones);

    return list;
}


linked_list transform_number(linked_list number_entry)
{
    uint64_t *number = number_entry->data;
    int n_digits = num_digits(*number);

    if (*number == 0) {
        *number = 1;
        return number_entry;
    }

    if (n_digits % 2) {
        *number *= 2024;
        return number_entry;
    }
    // Split the number
    uint64_t right = *number % modulus_map[n_digits >> 1];
    uint64_t left = *number / modulus_map[n_digits >> 1];
    *number = left;

    number_entry->next = linked_list_prepend(number_entry->next, make_uint64(right));   // Preserve node order
    return number_entry;
}

linked_list parse_input(char *string, uint64_t *list_len)
{
    uint64_t num;
    uint64_t len = 0;
    linked_list retlist = NULL;

    while (1 == sscanf(string, "%lu", &num)) {
        retlist = linked_list_append(retlist, make_uint64(num));
        len++;

        string = strchr(string, ' ');
        if (string == NULL)
            break;

        string++;
    }

    if (NULL != list_len)
        *list_len = len;

    return retlist;
}

#define CACHE_MAX 1000
uint64_t *len_cache[CACHE_MAX] = { NULL };

linked_list list_cache[CACHE_MAX];

void cache_destroy()
{
    for (int i = 0; i < CACHE_MAX; i++)
        if (NULL != len_cache[i]) {
            xfree(len_cache[i]);
            linked_list_destroy(list_cache[i], xfree);
            len_cache[i] = NULL;
            list_cache[i] = NULL;
        }
}

void cache_generate(uint64_t num)
{
    len_cache[num] = xmalloc(sizeof(uint64_t) * 26);

    char temp[3];
    sprintf(temp, "%lu", num);

    list_cache[num] = parse_input(temp, NULL);
    len_cache[num][0] = 1;

    for (int j = 1; j < 26; j++) {
        len_cache[num][j] = len_cache[num][j - 1];
        list_cache[num] = blink(list_cache[num], len_cache[num] + j);
    }
}

uint64_t cache_get_len(uint64_t num, int blinks)
{
    if (NULL == len_cache[num])
        cache_generate(num);

    return len_cache[num][blinks];
}

void *duplicate_number(void *temp)
{
    uint64_t *number = temp;

    return make_uint64(*number);
}

linked_list cache_get_list(uint64_t num)
{
    if (NULL == list_cache[num])
        cache_generate(num);

    return linked_list_duplicate(list_cache[num], duplicate_number);
}


// For problem 2 i need the depth first algorithm.
// Max recursion depth = number of blinks + original length of list
uint64_t depth_first(linked_list list, int blinks_left)
{
    if (NULL == list)
        return 0;

    uint64_t retval = 0;

    //Right first
    retval += depth_first(list->next, blinks_left);
    linked_list_destroy(list->next, xfree);
    list->next = NULL;

    if (blinks_left == 0) {
#if 0
        printf("%d: ", linked_list_length(debugger));
        linked_list_for_each(debugger, print_list, stdout);
        putchar('\n');
        getchar();
#endif
        return retval + 1;
    }

    uint64_t *number = list->data;

    //Shortcuts
    if (*number < CACHE_MAX) {
        if (blinks_left > 25) {
            linked_list temp = cache_get_list(*number);

            //Retain outside pointers
            free(list->data);
            list->data = temp->data;
            list->next = temp->next;
            free(temp);
            return retval + depth_first(list, blinks_left - 25);
        }

        return retval + cache_get_len(*number, blinks_left);
    }
    //Decend
    blinks_left--;

    transform_number(list);
    retval += depth_first(list, blinks_left);



    return retval;

}


int main(int argument_count, char **argument_vector)
{
    char *input = xload_file("input", NULL);

    linked_list list;
    uint64_t n_stones;

    list = parse_input(input, &n_stones);
    int n_blinks = 25;

#if 1
    if (argument_count == 2)
        n_blinks = atoi(argument_vector[1]);
#endif

    while (n_blinks) {
        list = blink(list, &n_stones);
        n_blinks--;
    }

    printf("Problem 1: %lu\n", n_stones);
    linked_list_destroy(list, xfree);


    //Problem 2 
    list = parse_input(input, NULL);

#if 0
    linked_list_for_each(list, print_list, stdout);
    putchar('\n');
    getchar();
#endif


    n_blinks = 75;
#if 1
    if (argument_count == 2)
        n_blinks = atoi(argument_vector[1]);
#endif


    n_stones = depth_first(list, n_blinks);
    printf("Problem 2: %lu\n", n_stones);
    linked_list_destroy(list, xfree);

    //  linked_list_for_each(cache_get_list(1), print_list, stdout);

    cache_destroy();

    return EXIT_SUCCESS;
}
