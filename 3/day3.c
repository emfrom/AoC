#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../utility_blob.c"


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
        int a, b;
        char c;

        if (3 == sscanf(temp, "%d,%d%c", &a, &b, &c) && c == ')')
            sum += a * b;
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

        int a, b;
        char c;
        if (3 == sscanf(temp, "%d,%d%c", &a, &b, &c) && c == ')')
            sum += a * b;
    }

    printf("Problem 2 sum is: %ld\n", sum);

    return EXIT_SUCCESS;
}
