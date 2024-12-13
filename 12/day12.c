#include <regex.h>
#include <stddef.h>
#include<stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include "../utility_blob.c"

// Notes day 12
//
// EXTERMELY overengineered. Starting to really feel at home in c again as well
// as writing everything by myself so falling into overengieering was
// inevitable. (or something)
//
// Liked my solution in the end, plus I also wrote a bunch of support funtions
// that I didnt use which will probably come in handy at some point.


struct extended_field_s {
    field base;
    void **extra;
};
typedef struct extended_field_s *extended_field;

extended_field extended_field_convert(field base)
{
    extended_field ext_field = xmalloc(sizeof(struct extended_field_s));

    ext_field->base = base;
    ext_field->extra = xmalloc(base->xsize * base->ysize * sizeof(void *));

    for (int i = 0; i < base->xsize * base->ysize; i++)
        ext_field->extra[i] = NULL;

    return ext_field;
}


void *extended_field_get(extended_field ext_field, int x, int y, char *c)
{
    if (NULL != c)
        *c = field_get(ext_field->base, x, y);

    return ext_field->extra[ext_field->base->xsize * y + x];
}

void *extended_field_set(extended_field ext_field, int x, int y,
                         void *extra)
{
    return ext_field->extra[ext_field->base->xsize * y + x] = extra;
}


void extended_field_map(extended_field ext_field,
                        void (*function)(extended_field field, int x,
                                         int y, void *), void *userdata)
{
    for (int y = 0; y < ext_field->base->ysize; y++)
        for (int x = 0; x < ext_field->base->ysize; x++)
            function(ext_field, x, y, userdata);
}

void extended_field_destroy_helper(extended_field ext, int x, int y,
                                   void *free_function_temp)
{
    if (NULL == free_function_temp)
        return;

    void *data = extended_field_get(ext, x, y, NULL);
    if (NULL == data)
        return;

    void (*free_function)(void *) = free_function_temp;

    free_function(data);
}

extended_field extended_field_destroy(extended_field ext_field,
                                      void (*free_func)(void *))
{
    if (NULL != free_func)
        extended_field_map(ext_field, extended_field_destroy_helper,
                           free_func);

    free(ext_field->extra);
    free(ext_field);

    return NULL;
}

extended_field extended_field_soliton = NULL;
void extended_field_soliton_destroy()
{
    extended_field_destroy(extended_field_soliton, NULL);
}

extended_field extended_field_soliton_get()
{
    if (NULL == extended_field_soliton) {
        field soliton = field_soliton_get();
        extended_field_soliton = extended_field_convert(soliton);

        atexit(extended_field_soliton_destroy);
    }

    return extended_field_soliton;
}

struct plot_s {
    int fence_length;
    char plant;
    linked_list region;
    int x;
    int y;
    char outside;
};

typedef struct plot_s *plot;

plot create_plot(char plant, int x, int y)
{
    plot new = xmalloc(sizeof(struct plot_s));

    new->fence_length = 0;
    new->plant = plant;
    new->region = NULL;
    new->x = x;
    new->y = y;
    new->outside = 0;

    return new;
}


const int dx[4] = { 1, 0, -1, 0 };
const int dy[4] = { 0, 1, 0, -1 };
const int dir_mask[4] = { 1, 2, 4, 8 };

void print_plot(plot dirtpad)
{
    printf("(%d,%d): %c ", dirtpad->x, dirtpad->y, dirtpad->plant);

    for (int i = 3; i >= 0; i--)
        printf("%c", dirtpad->outside & dir_mask[i] ? '1' : '0');

    putchar('\n');
}

void region_finder(extended_field garden, linked_list region,
                   char region_plant, int x, int y)
{
    if (!field_inbounds(garden->base, x, y))
        return;

    char plant;
    plot current = extended_field_get(garden, x, y, &plant);

    if (plant != region_plant)
        return;

    if (NULL == current) {
        current = create_plot(plant, x, y);
        extended_field_set(garden, x, y, current);
    }

    if (NULL != current->region)
        return;


    if (NULL == region)
        region = linked_list_append(NULL, current);
    else
        linked_list_append(region, current);
    current->region = region;

    for (int i = 0; i < 4; i++)
        region_finder(garden, region, region_plant, x + dx[i], y + dy[i]);

    return;
}

int same_plant_neighbour(int x, int y, int dir)
{
    field garden = field_soliton_get();

    if (!field_inbounds(garden, x, y))
        return 0;

    if (!field_inbounds(garden, x + dx[dir], y + dy[dir]))
        return 0;

    char plant = field_get(garden, x, y);


    if (plant != field_get(garden, x + dx[dir], y + dy[dir]))
        return 0;

    return 1;
}

void garden_analyzer(extended_field garden, int x, int y, void *data)
{
    linked_list *region_list = data;
    char plant;
    plot current = extended_field_get(garden, x, y, &plant);

    if (NULL == current) {
        current = create_plot(plant, x, y);
        extended_field_set(garden, x, y, current);
    }

    if (current->region == NULL) {
        region_finder(garden, NULL, plant, x, y);
        *region_list = linked_list_append(*region_list, current->region);
    }

    for (int dir = 0; dir < 4; dir++)
        if (!same_plant_neighbour(x, y, dir)) {
            current->fence_length += 1;
            current->outside |= dir_mask[dir];
        }
}

struct region_info {
    int fence_length;
    int area;
};

void collate_plots(void *data, void *userdata)
{
    plot current = data;
    struct region_info *info = userdata;

    info->fence_length += current->fence_length;
    info->area += 1;
}

void collate_region(void *data, void *userdata)
{
    linked_list region = data;
    int *cost = userdata;

    struct region_info info = { 0, 0 };
    linked_list_map(region, collate_plots, &info);

    *cost += info.area * info.fence_length;
}

#define ROTATE_CW(c) (((c) + 1) % 4)
#define ROTATE_CC(c) (((c) + 3) % 4)    // -1 == +3 in % 4

int side_finder(linked_list region)
{
    extended_field ext_field = extended_field_soliton_get();
    plot current;

    int sides = 0;

    while (NULL != region) {
        current = region->data;

        if (0 == current->outside) {
            region = linked_list_remove(region, current, NULL); //Only remove head
            continue;
        }

#if 0
        printf("List len: %d\n", linked_list_length(region));
        print_plot(current);
        getchar();
#endif

        int dir;

        current = region->data;

        for (dir = 0; !(current->outside & dir_mask[dir]); dir++);

        sides++;

        current->outside ^= dir_mask[dir];

        if (0 == current->outside)
            region = linked_list_remove(region, current, NULL);

        int x = current->x;
        int y = current->y;

        plot temp;

        int move_dir = ROTATE_CC(dir);
        while (same_plant_neighbour(x, y, move_dir)) {
            x = x + dx[move_dir];
            y = y + dy[move_dir];

            if (same_plant_neighbour(x, y, dir))
                break;

            temp = extended_field_get(ext_field, x, y, NULL);

            temp->outside ^= dir_mask[dir];
        }

        x = current->x;
        y = current->y;
        move_dir = ROTATE_CW(dir);
        while (same_plant_neighbour(x, y, move_dir)) {
            x = x + dx[move_dir];
            y = y + dy[move_dir];

            if (same_plant_neighbour(x, y, dir))
                break;

            temp = extended_field_get(ext_field, x, y, NULL);

            temp->outside ^= dir_mask[dir];
        }
    }

    return sides;
}

void problem2_region(void *data, void *userdata)
{
    linked_list region = data;
    int *cost = userdata;
    int list_len = linked_list_length(region);

    *cost += list_len * side_finder(linked_list_duplicate(region, NULL));
}


int main()
{
    extended_field garden = extended_field_soliton_get();

    linked_list region_list = NULL;
    extended_field_map(garden, garden_analyzer, &region_list);
    int cost = 0;

    linked_list_map(region_list, collate_region, &cost);

    printf("Problem 1: %d\n", cost);

    cost = 0;
    linked_list_map(region_list, problem2_region, &cost);
    printf("Problem 2: %d\n", cost);

    return EXIT_SUCCESS;
}
