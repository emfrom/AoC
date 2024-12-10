#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "../utility_blob.c"

// Notes day 10
//
// Straightforward path finding, recursive solution
//
// Problem 2 was solved by commenting out one line in the problem 1 solution


int trails_score(field area, int x, int y, cordinate *ends)
{
    char c = field_get(area, x, y);

#if 0
    field_set(area, x, y, '#');
    field_print(area);
    field_set(area, x, y, c);
    getchar();
#endif

    if (c == '9') {
#ifdef PROBLEM_1
        *ends = cordinate_chain(cordinate_create(x, y), *ends); //Sometimes
        field_set(area, x, y, '#');
#endif
        return 1;
    }

    const int dx[4] = { 1, -1, 0, 0 };
    const int dy[4] = { 0, 0, 1, -1 };
    int score = 0;

    c++;                        //Next

    for (int i = 0; i < 4; i++)
        if (field_inbounds(area, x + dx[i], y + dy[i]))
            if (c == field_get(area, x + dx[i], y + dy[i]))
                score += trails_score(area, x + dx[i], y + dy[i], ends);

    return score;
}

void find_trails(int x, int y, void *temp)
{
    field area = field_soliton_get();

    if ('0' != field_get(area, x, y))
        return;

    int *n_trails = temp;
    cordinate ends = NULL;

    *n_trails += trails_score(area, x, y, &ends);

    while (ends != NULL) {
        cordinate temp = ends;
        field_set(area, ends->x, ends->y, '9');

        ends = ends->next;
        xfree(temp);
    }
}


int main()
{
    field island = field_soliton_get();

#if 0
    field_print(island);
    printf("\n");
#endif

    int trails_total_score = 0;
    field_for_all(island, find_trails, &trails_total_score);

    printf("Problem 1: %d\n", trails_total_score);


    return EXIT_SUCCESS;
}
