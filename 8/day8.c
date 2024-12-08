#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "../utility_blob.c"

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

int xsize, ysize;

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

    while (x >= 0 && y >= 0 && x < xsize && y < ysize) {
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



int main()
{
    int n_lines;
    char **input = xload_lines("input", &n_lines);


    xsize = strlen(input[0]);
    ysize = n_lines;

    cordinate tower_list[N_FREQUENCIES] = { NULL };
    for (int y = 0; y < ysize; y++)
        for (int x = 0; x < xsize; x++)
            if (isalnum(input[y][x])) {
                int index = frequency_index(input[y][x]);

                tower_list[index] =
                    cordinate_chain(cordinate_create(x, y),
                                    tower_list[index]);

            }

    cordinate antinode_cords = NULL;
    for (int i = 0; i < N_FREQUENCIES; i++)
        if (tower_list[i] != NULL) {
            cordinate temp_outer = tower_list[i];
            cordinate temp_inner;

            while (temp_outer != NULL) {
                temp_inner = temp_outer->next;

                while (temp_inner != NULL) {
                    cordinate antinode;

                    antinode =
                        cordinate_get_antinode(temp_outer, temp_inner);
                    antinode_cords =
                        cordinate_chain(antinode, antinode_cords);

                    antinode =
                        cordinate_get_antinode(temp_inner, temp_outer);
                    antinode_cords =
                        cordinate_chain(antinode, antinode_cords);

                    temp_inner = temp_inner->next;
                }

                temp_outer = temp_outer->next;
            }
        }
    //Plot them to remove duplicates
    while (NULL != antinode_cords) {
        int x, y;
        x = antinode_cords->x;
        y = antinode_cords->y;

        if (x >= 0 && y >= 0 && x < xsize && y < ysize)
            input[y][x] = '#';

        cordinate temp = antinode_cords;
        antinode_cords = antinode_cords->next;
        xfree(temp);
    }

    int num_antinodes = 0;

    for (int y = 0; y < ysize; y++) {
#if 0
        printf("%s\n", input[y]);
#endif

        for (int x = 0; x < xsize; x++)
            if (input[y][x] == '#')
                num_antinodes++;
    }

    printf("Number of antinodes(p1): %d\n", num_antinodes);

    //Problem 2
    num_antinodes = 0;
    antinode_cords = NULL;

    for (int i = 0; i < N_FREQUENCIES; i++)
        if (tower_list[i] != NULL) {
            cordinate temp_outer = tower_list[i];
            cordinate temp_inner;

            while (temp_outer != NULL) {
                temp_inner = temp_outer->next;

                while (temp_inner != NULL) {
                    cordinate antinode;

                    antinode =
                        cordinate_get_antinodes(temp_outer, temp_inner);
                    if (NULL != antinode)
                        antinode_cords =
                            cordinate_chain(antinode, antinode_cords);

                    antinode =
                        cordinate_get_antinodes(temp_inner, temp_outer);
                    if (NULL != antinode)
                        antinode_cords =
                            cordinate_chain(antinode, antinode_cords);

                    temp_inner = temp_inner->next;
                }

                temp_outer = temp_outer->next;
            }

            temp_outer = tower_list[i];
            while (NULL != temp_outer) {
                temp_inner = temp_outer;
                temp_outer = temp_outer->next;
                xfree(temp_inner);
            }
        }
    //Plot them to remove duplicates
    while (NULL != antinode_cords) {
        int x, y;
        x = antinode_cords->x;
        y = antinode_cords->y;

        if (x >= 0 && y >= 0 && x < xsize && y < ysize)
            input[y][x] = '#';

        cordinate temp = antinode_cords;
        antinode_cords = antinode_cords->next;
        xfree(temp);
    }

    for (int y = 0; y < ysize; y++) {
#if 0
        printf("%s\n", input[y]);
#endif

        for (int x = 0; x < xsize; x++)
            if (input[y][x] == '#')
                num_antinodes++;
    }

    printf("Number of antinodes(p2): %d\n", num_antinodes);

    xfree(input);

    return EXIT_SUCCESS;
}
