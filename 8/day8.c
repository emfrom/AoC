#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "../utility_blob.c"

// Notes: Day 8
// (On the road and very tired)
//
// Not a pretty solutions but it worked 
// Tried to prettyfiy it and broke it so, sleeptime.

typedef struct cordinate_s *cordinate;
struct cordinate_s {
    int x;
    int y;

    cordinate next;
};


cordinate cordinate_create(int x, int y)
{
    cordinate cord = xmalloc(sizeof(struct cordinate_s));

    cord->x = x;
    cord->y = y;

    cord->next = NULL;

    return cord;
}

cordinate cordinate_chain(cordinate first, cordinate second)
{
    cordinate temp = first;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = second;

#if 0
    temp = first;
    int count = 0;
    while (temp != NULL) {
        temp = temp->next;
        count++;
    }
    printf("Chained %d coordinates\n", count);
#endif

    return first;
}

cordinate cordinate_get_antinode(cordinate cordA, cordinate cordB)
{
    int dx;
    int dy;

    dx = cordA->x - cordB->x;
    dy = cordA->y - cordB->y;

    return cordinate_create(cordA->x + dx, cordA->y + dy);
}


cordinate cordinate_get_antinodes(cordinate cordA, cordinate cordB)
{
    int dx;
    int dy;

    dx = cordA->x - cordB->x;
    dy = cordA->y - cordB->y;

    int x = cordA->x;
    int y = cordA->y;
    cordinate antinodes = NULL;

    int count = 0;
    field area = field_soliton_get();

    while (field_inbounds(area, x, y)) {
        antinodes = cordinate_chain(cordinate_create(x, y), antinodes);

        count++;

        x += dx;
        y += dy;
    }

#if 0
    printf("Found %d antinodes\n", count);
#endif

    return antinodes;
}


int frequency_index(char c)
{
    return ((c) >= 'a' && (c) <= 'z' ? (c) - 'a'
            : (c) >= 'A' && (c) <= 'Z' ? (c) - 'A' + 26
            : (c) >= '0' && (c) <= '9' ? (c) - '0' + 52 : -1);
}

#define N_FREQUENCIES 62

cordinate create_antinode_list(cordinate *list, int n_list,
                               cordinate(*function) (cordinate A,
                                                     cordinate B))
{
    cordinate cords = NULL;
    for (int i = 0; i < n_list; i++)
        if (list[i] != NULL) {
            cordinate temp_outer = list[i];
            cordinate temp_inner;

            while (temp_outer != NULL) {
                temp_inner = temp_outer->next;

                while (temp_inner != NULL) {
                    cordinate antinode;

                    antinode = function(temp_outer, temp_inner);
                    cords = cordinate_chain(antinode, cords);

                    antinode = function(temp_inner, temp_outer);
                    cords = cordinate_chain(antinode, cords);

                    temp_inner = temp_inner->next;
                }

                temp_outer = temp_outer->next;
            }
        }
    return cords;
}

int main()
{
    field area;
    area = field_soliton_get();


    cordinate tower_list[N_FREQUENCIES] = { NULL };
    for (int y = 0; field_inbounds(area, 0, y); y++)
        for (int x = 0; field_inbounds(area, x, 0); x++)
            if (isalnum(field_get(area, x, y))) {
                int index = frequency_index(field_get(area, x, y));

                tower_list[index] =
                    cordinate_chain(cordinate_create(x, y),
                                    tower_list[index]);

            }

    cordinate antinode_cords;
    antinode_cords =
        create_antinode_list(tower_list, N_FREQUENCIES,
                             cordinate_get_antinode);


    //Plot them to remove duplicates
    while (NULL != antinode_cords) {
        int x, y;
        x = antinode_cords->x;
        y = antinode_cords->y;

        if (field_inbounds(area, x, y))
            field_set(area, x, y, '#');

        cordinate temp = antinode_cords;
        antinode_cords = antinode_cords->next;
        xfree(temp);
    }

    int num_antinodes = 0;
    for (int y = 0; field_inbounds(area, 0, y); y++)
        for (int x = 0; field_inbounds(area, x, 0); x++)
            if (field_get(area, x, y) == '#')
                num_antinodes++;

    printf("Number of antinodes(p1): %d\n", num_antinodes);



    //Problem 2
    num_antinodes = 0;
    antinode_cords =
        create_antinode_list(tower_list, N_FREQUENCIES,
                             cordinate_get_antinodes);

    //Plot them to remove duplicates
    while (NULL != antinode_cords) {
        int x, y;
        x = antinode_cords->x;
        y = antinode_cords->y;

        if (field_inbounds(area, x, y))
            field_set(area, x, y, '#');

        cordinate temp = antinode_cords;
        antinode_cords = antinode_cords->next;
        xfree(temp);
    }


    for (int y = 0; field_inbounds(area, 0, y); y++)
        for (int x = 0; field_inbounds(area, x, 0); x++)
            if (field_get(area, x, y) == '#')
                num_antinodes++;

    printf("Number of antinodes(p2): %d\n", num_antinodes);


    for (int i = 0; i < N_FREQUENCIES; i++)
        if (tower_list[i] != NULL)
            xfree(tower_list[i]);


    return EXIT_SUCCESS;
}
