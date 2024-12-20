#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "../utility_blob.c"
#include <stdbool.h>

// Directions
// Diagonals 0-4
const int dx[8] = { -1, 1, 1, -1, -1, 0, 1, 0 };
const int dy[8] = { -1, -1, 1, 1, 0, -1, 0, 1 };

//Field of play, assign once
char **playarea;
int xsize;
int ysize;

bool valid_coordinate(int x, int y)
{
    return !(y < 0 || x < 0 || y >= ysize || x >= xsize);
}

// Recursive search per direction
bool match_mas(int dir, int step, int x, int y)
{
    const char *target_string = "MAS";

    if (step == 3)
        return true;

    if (!match_mas(dir, step + 1, x + dx[dir], y + dy[dir]))
        return false;

    if (!valid_coordinate(x, y))
        return false;

    return playarea[y][x] == target_string[step];
}

int check_coordinate_xmas(int x, int y)
{

    if (playarea[y][x] != 'X')
        return 0;

    int num_matches = 0;
    for (int i = 0; i < 8; i++)
        if (match_mas(i, 0, x + dx[i], y + dy[i]))
            num_matches++;

    return num_matches;
}

int check_coordinate_x_mas(int x, int y)
{

    if (playarea[y][x] != 'A')
        return 0;

    int num_matches = 0;
    for (int i = 0; i < 4; i++)
        if (match_mas(i, 0, x - dx[i], y - dy[i])) {
            num_matches++;
            if (num_matches == 2)
                return 1;
        }

    return 0;
}

int main()
{

    int lines;
    playarea = xload_lines("input", &lines);

    ysize = lines;
    xsize = strlen(playarea[0]);

#if 0
    for (int i = 1; i < ysize; i++)
        if (xsize != (int) strlen(playarea[i])) {
            fprintf(stderr, "main() -> uneven line length\n");
            exit(EXIT_FAILURE);
        }
#endif

    //Part 1
    int num_xmas = 0;
    for (int y = 0; y < ysize; y++)
        for (int x = 0; x < xsize; x++)
            num_xmas += check_coordinate_xmas(x, y);

    printf("Number of xmas:es: %d\n", num_xmas);


    //Part 2
    num_xmas = 0;
    for (int y = 0; y < ysize; y++)
        for (int x = 0; x < xsize; x++)
            num_xmas += check_coordinate_x_mas(x, y);

    printf("Number of x-mas:es: %d\n", num_xmas);

    return EXIT_SUCCESS;
}
