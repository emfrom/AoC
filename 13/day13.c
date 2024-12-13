#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "../utility_blob.c"

void print_matrix(int *M)
{
    printf("⌈ %d %d ⌉\n", M[0], M[1]);
    printf("⌊ %d %d ⌋\n", M[2], M[3]);
}


// Assume int solution always exists
//  it's AoC, it probably does
void int_gauss(int *A, int *B, int *x)
{
    int factor = A[2] / A[0];
    A[2] = 0;
    A[3] -= factor * A[1];
    B[1] -= factor * B[0];
    x[1] = B[1] / A[3];
    x[0] = (B[0] - A[1] * x[1]) / A[0];
}


int main()
{
    int A[4] = { 94, 34, 22, 67 };
    int B[2] = { 8400, 5400 };
    int x[2] = { 0 };

    print_matrix(A);

    int_gauss(A, B, x);

    print_matrix(A);

    printf("Solution: %d %d\n", x[0], x[1]);

    return EXIT_SUCCESS;
}
