#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../utility_blob.c"

void print_matrix(double *M)
{
    printf("⌈ %f %f ⌉\n", M[0], M[1]);
    printf("⌊ %f %f ⌋\n", M[2], M[3]);
}

void print_augmatrix(double *A, double *B)
{
    printf("⌈ %f %f | %f ⌉\n", A[0], A[1], B[0]);
    printf("⌊ %f %f | %f ⌋\n", A[2], A[3], B[1]);
}

const uint64_t prime_product = 614889782588491410;

void swap(uint64_t *a, uint64_t *b)
{
    uint64_t temp = *a;
    *a = *b;
    *b = temp;
}

void swap_rows(uint64_t *A, uint64_t *B)
{
    swap(A, A + 2);
    swap(A + 1, A + 3);
    swap(B, B + 1);
}


int invertible(double *A)
{
    //det(A) != 0
    return (A[0] * A[3] - A[1] * A[2]) < 0.0001f;
}

int gauss(double *A, double *B, double *x)
{
    if (A[0] == 0)
        return false;

    double factor = A[2] / A[0];
    A[2] = 0;
    A[3] -= factor * A[1];
    B[1] -= factor * B[0];

    if (A[3] == 0)
        return false;

    x[1] = B[1] / A[3];
    x[0] = (B[0] - A[1] * x[1]) / A[0];

    return true;
}



char *parse_input(char *input, double *A, double *B)
{
    const char *format =
        "Button A: X+%d, Y+%d\nButton B: X+%d, Y+%d\nPrize: X=%d, Y=%d";
    int a[4];
    int b[2];

    if (NULL == input || 6 != sscanf(input, format, a, a + 2, a + 1, a + 3, b, b + 1))  //Flipped
        return NULL;

    A[0] = (double) a[0];
    A[1] = (double) a[1];
    A[2] = (double) a[2];
    A[3] = (double) a[3];
    B[0] = (double) b[0];
    B[1] = (double) b[1];


    return strstr(input + 1, "Button A: X+");
}

int cost_function(double *x)
{
    return (int) 3 *x[0] + x[1];
}

int main()
{
    char *input = xload_file("input", NULL);
    double A[4];
    double B[2];
    double x[2];
    char *temp = input;

    int total_cost = 0;

    while (NULL != temp) {
        temp = parse_input(temp, A, B);
        int cost = UINT32_MAX;

        for (int x0 = 0; x0 < 100; x0++)
            for (int x1 = 0; x1 < 100; x1++)
                if (0.0001f > (A[0] * x0 + A[1] * x1 - B[0]) &&
                    0.0001f > (A[2] * x0 + A[2] * x1 - B[1])) {
                    x[0] = x0;
                    x[1] = x1;
                    if (cost > cost_function(x))
                        cost = cost_function(x);
                }


        /*
           if(!invertible(A)) {
           printf("Not unique\n");
           print_augmatrix(A,B);
           }

           if(!gauss(A,B,x))
           continue;



           if(0 > x[0] || 100 < x[0] ||
           0 > x[1] || 100 < x[1])
           continue;

           cost =  cost_function(x);
           //printf("Solution: %f %f\n",x[0], x[1]);
           // printf("Valid solution, cost is: %d\n", cost);
         */
        total_cost += cost;
    }

    printf("\nTotal cost: %d\n", total_cost);

    return EXIT_SUCCESS;
}
