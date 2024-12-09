#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "../utility_blob.c"

// Notes on day 6
//
// I had a solution that looked great, didnt work
// I tried another way, didnt work
// Created this solution, a by the book version
//
// This solution didnt work either
// Until, I changed to brute force the entire field
//
// Before I only checked the squares where the guard passe in the unaltered
// field. Super weird, how can a square where he doesnt go change the outcome?
//
// Anyway, this solution is bastardly slow, it checks placing a # on every
// square. It creates and destroys all data structures every time.
//
// .. but it works. Sigh..


struct guard_s {
    int16_t x;
    int16_t y;
    int8_t direction;
    field playarea;
};
typedef struct guard_s *guard;

void guard_print_status(guard lawman)
{
    for (int i = 0; i < lawman->playarea->ysize; i++)
        printf("%s\n", lawman->playarea->layout[i]);

    printf("\n");
}

guard guard_create()
{
    guard lawman = xmalloc(sizeof(struct guard_s));

    lawman->playarea = field_create();

    lawman->direction = 0;

    for (int y = 0; y < lawman->playarea->ysize; y++)
        for (int x = 0; x < lawman->playarea->xsize; x++)
            if (lawman->playarea->layout[y][x] == '^') {
                lawman->x = x;
                lawman->y = y;

                return lawman;
            }

    fprintf(stderr, "guard_create() -> Couldnt find caret\n");
    exit(EXIT_FAILURE);
}

const char GUARD_VISITED = 'x';
const char guard_icon[4] = { '^', '>', 'v', '<' };
const int dx[4] = { 0, 1, 0, -1 };
const int dy[4] = { -1, 0, 1, 0 };

guard guard_turn(guard lawman)
{
    lawman->direction = (lawman->direction + 1) % 4;
    return lawman;
}

void guard_destroy(guard lawman)
{
    field_destroy(lawman->playarea);
    xfree(lawman);
}

guard guard_move(guard lawman)
{
    //0 is 12 oclock

    int newx, newy;
    newx = lawman->x + dx[lawman->direction];
    newy = lawman->y + dy[lawman->direction];

    if (!field_pos_inbounds(lawman->playarea, newx, newy)) {
        return NULL;
    }

    while (field_pos_blocked(lawman->playarea, newx, newy)) {
        guard_turn(lawman);

        newx = lawman->x + dx[lawman->direction];
        newy = lawman->y + dy[lawman->direction];
    }

    //Mark old position
    lawman->playarea->layout[lawman->y][lawman->x] = GUARD_VISITED;
#if 0
    //Mark new position
    lawman->playarea->layout[newy][newx] = guard_icon[lawman->direction];
#endif

    lawman->x = newx;
    lawman->y = newy;

    return lawman;
}

guard guard_copy(guard copy, guard original)
{
    copy->direction = original->direction;
    copy->x = original->x;
    copy->y = original->y;

    copy->playarea = field_copy(copy->playarea, original->playarea);

    return copy;
}

int guard_positions_visited(guard gone)
{
    field end_state = gone->playarea;
    int n_positions = 0;

    for (int y = 0; y < end_state->ysize; y++)
        for (int x = 0; x < end_state->xsize; x++)
            if (end_state->layout[y][x] == GUARD_VISITED)
                n_positions++;

    return n_positions;
}


struct guard_state_s {
    int16_t x;
    int16_t y;
    int8_t direction;
};
typedef struct guard_state_s *guard_state;


guard_state guard_state_create(int x, int y, int direction)
{
    guard_state state = xmalloc(sizeof(struct guard_state_s));

    state->x = x;
    state->y = y;
    state->direction = direction;

    return state;
}

guard_state guard_state_create_from_guard(guard lawman)
{
    return guard_state_create(lawman->x, lawman->y, lawman->direction);
}

struct guard_states_s {
    int n_elements;

    //universe is small enough
    guard_state *direct_access_table;
    field playarea;
};
typedef struct guard_states_s *guard_states;

guard_states guard_states_create(field playarea)
{
    guard_states states = xmalloc(sizeof(struct guard_states_s));

    states->playarea = playarea;

    states->n_elements = playarea->xsize * playarea->ysize * 4;

    states->direct_access_table =
        xmalloc(sizeof(guard_state) * states->n_elements);

    for (int i = 0; i < states->n_elements; i++)
        states->direct_access_table[i] = NULL;

    return states;
}

int guard_state_dat_index(guard_states states, guard_state state)
{
    field playfield = states->playarea;

    return ((state->direction * playfield->xsize) +
            state->x) * playfield->ysize + state->y;
}

guard_states guard_states_add(guard_states states, guard_state state)
{
    states->direct_access_table[guard_state_dat_index(states, state)] =
        state;
    return states;
}

int guard_states_check(guard_states states, guard_state state)
{
    int index = guard_state_dat_index(states, state);

    if (NULL == states->direct_access_table[index])
        return false;

    return true;
}

guard_states guard_states_destroy(guard_states states)
{

    for (int i = 0; i < states->n_elements; i++)
        if (states->direct_access_table[i] != NULL)
            xfree(states->direct_access_table[i]);

    xfree(states);

    return NULL;
}

int main()
{
    guard lawman = guard_create();

    //Problem 1
    while (NULL != guard_move(lawman));

    int n_positions = guard_positions_visited(lawman);
    printf("Guard visited %d positions\n", n_positions);
    guard_print_status(lawman);

    //Problem 2
    guard deputy = guard_create();
    int n_loops = 0;

    for (int y = 0; y < deputy->playarea->ysize; y++)
        for (int x = 0; x < deputy->playarea->xsize; x++)
            if (lawman->playarea->layout[y][x] != '#') {        //Cant add obstacles where they already exist

                deputy->playarea->layout[y][x] = '#';
                guard_states states =
                    guard_states_create(deputy->playarea);

                while (NULL != guard_move(deputy)) {
                    guard_state deputy_state =
                        guard_state_create_from_guard(deputy);
                    if (guard_states_check(states, deputy_state)) {
                        //Loop
                        xfree(deputy_state);
                        n_loops++;
#if 1
                        printf("New loop: %d,%d (total: %d)\n", x, y,
                               n_loops);
#endif
                        break;
                    }

                    states = guard_states_add(states, deputy_state);
                }

                guard_states_destroy(states);
                guard_destroy(deputy);
                deputy = guard_create();
            }
    //Actually find the loops

    printf("Number of possible loops from one new obstacle: %d\n",
           n_loops);

    guard_destroy(lawman);      //I shot the sheriff,
    //guard_destroy(deputy); //But I didntr shoot the deputyyyy.
    return EXIT_SUCCESS;
}
