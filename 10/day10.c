#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "../utility_blob.c"


int trails_score(field area, int x, int y)
{
    char c = field_get(area, x, y);

#if 1
    field_set(area, x, y, '#');
    field_print(area);
    field_set(area, x, y, c);
    getchar();
#endif

    if (c == '9') {
        field_set(area, x, y, '#');
        return 1;
    }

    const int dx[4] = { 1, -1, 0, 0 };
    const int dy[4] = { 0, 0, 1, -1 };
    int score = 0;

    c++;                        //Next

    for (int i = 0; i < 4; i++)
        if (field_inbounds(area, x + dx[i], y + dy[i]))
            if (c == field_get(area, x + dx[i], y + dy[i]))
                score += trails_score(area, x + dx[i], y + dy[i]);

    return score;
}

void find_trails(int x, int y, void *temp)
{
    field area = field_soliton_get();
    int *n_trails = temp;

    if ('0' == field_get(area, x, y))
        *n_trails += trails_score(area, x, y);

}


int main()
{
    field island = field_soliton_get();

#if 1
    field_print(island);
    printf("\n");
#endif

    int trails_total_score = 0;
    field_for_all(island, find_trails, &trails_total_score);

    printf("Problem 1: %d\n", trails_total_score);


    return EXIT_SUCCESS;
}
