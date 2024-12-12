#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "../utility_blob.c"


// Notes on day 11
// Backed myself far into a corner by not reaching for the hashmap right away
// I think I'm just too lazy to write a one, but here we are.
//
// This is ring transformation with some 4k numbers at 75 generations
// So, hash not even needed but it would have let me see how few
// numbers there really were. I'm writing one now so I wont fall into the lazy
// pit anymore this AoC


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
// only need half (splitting numbers)
const uint64_t modulus_map[11] = { 1, 10, 100, 1000,
    10000, 100000, 1000000, 10000000,
    100000000, 1000000000, 10000000000
};

//
// Heart of Problem 1 solution
// 
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

linked_list parse_input(char *string, uint64_t *list_len)
{
    uint64_t num;
    uint64_t len = 0;
    linked_list retlist = NULL;

    while (1 == sscanf(string, "%lu", &num)) {
        retlist = linked_list_prepend(retlist, make_uint64(num));
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

typedef struct stone_s *stone;
struct stone_s {
    uint64_t number;

    stone next_gen;
    stone next_split;

    uint64_t n_thisgen;
    uint64_t n_nextgen;
};

uint64_t stone_hash(void *temp)
{
    stone everest = temp;
    uint64_t number = everest->number;

    return hash_u64(&number);
}

linked_list stone_pool = NULL;
hash_table stone_map = NULL;


stone stone_populate_pointer(uint64_t number)
{
    stone rock = hash_table_find(stone_map, &number);

    if (NULL != rock)
        return rock;

    //New stone
    rock = xmalloc(sizeof(struct stone_s));
    rock->number = number;
    rock->next_gen = NULL;
    rock->next_split = NULL;
    rock->n_thisgen = 1;
    rock->n_nextgen = 0;

    hash_table_insert(stone_map, rock);
    stone_pool = linked_list_prepend(stone_pool, rock);

    return rock;
}

void parse_num_worker(void *temp, void *user)
{
    uint64_t *number;
    number = user;              //stfu compiler
    number = temp;

    stone_populate_pointer(*number);    //Adds iteself to the pool
}


linked_list parse_num_to_pool(linked_list numbers)
{

    linked_list_map(numbers, parse_num_worker, NULL);

    return stone_pool;
}

// A blink consists of thee phases
// 1. Populate the pointers
// 2. Calculate into n_next
// 3. Move n_nextgen to n_thisgen (and 0 nextgen);
//
// Doing each as a linked list map call
void blink_populate_pointers(void *data, void *userdata)
{
    stone numbered_stone = data;
    uint64_t *newly_created = userdata;

    if (numbered_stone->next_gen != NULL)
        return;

    //Newly encountered number
    if (NULL != userdata)
        *newly_created += 1;

    uint64_t number = numbered_stone->number;

    if (number == 0) {
        numbered_stone->next_gen = stone_populate_pointer(1);
        return;
    }

    int n_digits = num_digits(number);

    if (n_digits % 2) {
        numbered_stone->next_gen = stone_populate_pointer(2024 * number);
        return;
    }
    //Split the number
    uint64_t right = number % modulus_map[n_digits >> 1];
    uint64_t left = number / modulus_map[n_digits >> 1];

    numbered_stone->next_gen = stone_populate_pointer(right);
    numbered_stone->next_split = stone_populate_pointer(left);

    return;
}

void blink_calculate(void *data, void *userdata)
{
    stone numbered_stone;
    numbered_stone = userdata;  //only for the compiler
    numbered_stone = data;

    if (numbered_stone->next_gen == NULL)       //New stones still have NULLs
        return;

    numbered_stone->next_gen->n_nextgen += numbered_stone->n_thisgen;

    if (numbered_stone->next_split == NULL)
        return;

    numbered_stone->next_split->n_nextgen += numbered_stone->n_thisgen;
    return;
}

void blink_move_counts(void *data, void *userdata)
{
    uint64_t *total_count = userdata;
    stone numbered_stone = data;

    numbered_stone->n_thisgen = numbered_stone->n_nextgen;

    if (NULL != userdata)
        *total_count += numbered_stone->n_thisgen;

    numbered_stone->n_nextgen = 0;
}

void print_stone(void *data, void *userdata)
{
    stone rocky = data;
    FILE *fp = userdata;

    fprintf(fp, " %lu:%lu", rocky->number, rocky->n_thisgen);
}

int main(int argument_count, char **argument_vector)
{
    uint64_t problem_1 = 0;
    uint64_t problem_2 = 0;

    char *input = xload_file("input", NULL);

    int n_blinks = 75;
    if (argument_count == 2)
        n_blinks = atoi(argument_vector[1]);

    linked_list list;
    list = parse_input(input, &problem_1);


    //Problem 2 stuff
    stone_map = hash_table_create(100000000, stone_hash);
    parse_num_to_pool(list);
    uint64_t n_numbers = 0;

    for (int i = 1; i <= n_blinks; i++) {
        printf("Generation: %d\n-------------------\n", i);

        linked_list_map(stone_pool, blink_populate_pointers, &n_numbers);
        printf("Total numbers: %lu\n", n_numbers);


        linked_list_map(stone_pool, blink_calculate, NULL);

        problem_2 = 0;
        linked_list_map(stone_pool, blink_move_counts, &problem_2);

        if (i < 26) {
            list = blink(list, &problem_1);
            printf("Total stones(p1): %lu\n", problem_1);
        }
        printf("Total stones(p2): %lu\n\n", problem_2);

#if 0

        printf("Problem 1: ");
        linked_list_map(list, print_list, stdout);
        printf("\nProblem 2: ");
        linked_list_map(stone_pool, print_stone, stdout);
        printf("\n");

        getchar();
#endif
    }

    printf("Problem 1: %lu\n", problem_1);
    printf("Problem 2: %lu\n", problem_2);


    //Trash it
    hash_table_destroy(stone_map, NULL);
    linked_list_destroy(stone_pool, xfree);
    linked_list_destroy(list, xfree);
    xfree(input);

    return EXIT_SUCCESS;
}
