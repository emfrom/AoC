#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "../utility_blob.c"


// match on [0-9]+,[0-9]+\)
// Using a regex lib would make this function twice as big (minimum)
int quick_check(char *expr)
{
    if (!isdigit(*expr))
        return 0;

    while (isdigit(*expr))
        expr++;

    if (*expr != ',')
        return 0;

    expr++;
    if (!isdigit(*expr))
        return 0;

    while (isdigit(*expr))
        expr++;

    if (*expr != ')')
        return 0;

    return 1;
}

int main()
{

    //Load input (as char blob)
    size_t size;
    char *input = xload_file("input", &size);


    // Problem 1
    uint64_t sum = 0;
    char *temp = input;

    while (NULL != (temp = strstr(temp, "mul("))) {
        temp += 4;

        if (quick_check(temp)) {
            int a, b;

            if (2 == sscanf(temp, "%d,%d", &a, &b)) {
#if 0
                printf("%.*s\n", 15, temp);
#endif
                sum += a * b;
            }
        }
    }

    printf("Problem 1 sum is: %ld\n", sum);



    // Problem 2
    sum = 0;
    temp = input;

    char *end_of_file = input + size;
    int mult_active = 1;
    const char *switch_string[] = { "do()", "don't()" };
    char *next_switch = strstr(input, switch_string[mult_active]);

    while (NULL != (temp = strstr(temp, "mul("))) {
        temp += 4;

        while (temp > next_switch) {
            mult_active = (mult_active + 1) % 2;

            next_switch = strstr(next_switch, switch_string[mult_active]);

            if (NULL == next_switch)
                next_switch = end_of_file;
        }

        if (!mult_active)
            continue;

        if (quick_check(temp)) {
            int a, b;

            if (2 == sscanf(temp, "%d,%d", &a, &b))
                sum += a * b;
        }
    }

    printf("Problem 2 sum is: %ld\n", sum);

    return EXIT_SUCCESS;
}
