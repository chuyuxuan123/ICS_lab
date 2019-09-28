/**
 * name:ChuYuxuan
 * ID:517030910079
 */

/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int t1, t2, t3, t4, t5, t6, t7, t8;
    int i, j;
    int block_row, block_col;
    if (M == 32)
    {
        // use 8*8 block
        for (block_row = 0; block_row < 32; block_row += 8)
        {
            for (block_col = 0; block_col < 32; block_col += 8)
            {
                for (i = block_row; i < block_row + 8; ++i)
                {
                    for (j = block_col; j < block_col + 8; ++j)
                    {
                        if (i != j)
                        {
                            B[j][i] = A[i][j];
                        }
                        else
                        {
                            t1 = A[i][j];
                            t2 = i;
                        }
                    }
                    if (block_col == block_row)//diagonal
                    {
                        B[t2][t2] = t1;
                    }
                }
            }
        }
    }
    else if (M == 64)
    {
        /**
         * use 8*8 block
         * use loacl variables to avoid cache miss
         * step 1:
         * ------------          -------------
         * |    |     |          |     |      |
         * | A1 |  A2 |          |  A1'|  A2' |
         * ------------  ====>   --------------
         * |    |     |          |     |      |
         * | A3 |  A4 |          |     |      |
         * ------------          --------------
         * step 2:
         * ------------          --------------             --------------
         * |    |     |          |     |      |             |     |      |
         * | A1 |  A2 |          |  A1'|  A2' |             |  A1'|  A3' |
         * ------------  ====>   --------------    ======>  --------------
         * |    |     |          |     |      |             |     |      |
         * | A3 |  A4 |          |  A3'|      |             |  A2'|      |
         * ------------          --------------             --------------
         * step 3:
         * ------------          -------------
         * |    |     |          |     |      |
         * | A1 |  A2 |          |  A1'|  A3' |
         * ------------  ====>   --------------
         * |    |     |          |     |      |
         * | A3 |  A4 |          |  A2'|  A4' |
         * ------------          --------------
         */
        for (block_row = 0; block_row < N; block_row += 8)
        {
            for (block_col = 0; block_col < M; block_col += 8)
            {
                for (i = block_row; i < block_row + 4; i++)
                {
                    j = block_col;
                    t1 = A[i][j + 0];
                    t2 = A[i][j + 1];
                    t3 = A[i][j + 2];
                    t4 = A[i][j + 3];
                    t5 = A[i][j + 4];
                    t6 = A[i][j + 5];
                    t7 = A[i][j + 6];
                    t8 = A[i][j + 7];

                    B[j + 0][i] = t1;
                    B[j + 1][i] = t2;
                    B[j + 2][i] = t3;
                    B[j + 3][i] = t4;

                    B[j + 0][i + 4] = t5;
                    B[j + 1][i + 4] = t6;
                    B[j + 2][i + 4] = t7;
                    B[j + 3][i + 4] = t8;
                }
                for (j = block_col; j < block_col + 4; j++)
                {
                    i = block_row;
                    t1 = A[i + 4][j];
                    t2 = A[i + 5][j];
                    t3 = A[i + 6][j];
                    t4 = A[i + 7][j];

                    t5 = B[j][i + 4];
                    t6 = B[j][i + 5];
                    t7 = B[j][i + 6];
                    t8 = B[j][i + 7];

                    B[j][i + 4] = t1;
                    B[j][i + 5] = t2;
                    B[j][i + 6] = t3;
                    B[j][i + 7] = t4;

                    B[j + 4][i + 0] = t5;
                    B[j + 4][i + 1] = t6;
                    B[j + 4][i + 2] = t7;
                    B[j + 4][i + 3] = t8;
                }

                for (i = block_row + 4; i < block_row + 8; i += 2)
                {
                    j = block_col + 4;
                    t1 = A[i][j + 0];
                    t2 = A[i][j + 1];
                    t3 = A[i][j + 2];
                    t4 = A[i][j + 3];

                    t5 = A[i + 1][j + 0];
                    t6 = A[i + 1][j + 1];
                    t7 = A[i + 1][j + 2];
                    t8 = A[i + 1][j + 3];

                    B[j + 0][i] = t1;
                    B[j + 1][i] = t2;
                    B[j + 2][i] = t3;
                    B[j + 3][i] = t4;

                    B[j + 0][i + 1] = t5;
                    B[j + 1][i + 1] = t6;
                    B[j + 2][i + 1] = t7;
                    B[j + 3][i + 1] = t8;
                }
            }
        }
    }
    else if (M == 61)
    {
        //use 16*16 block
        for (block_row = 0; block_row < N; block_row += 16)
        {
            for (block_col = 0; block_col < M; block_col += 16)
            {
                for (i = block_row; (i < block_row + 16) && (i < N); ++i)
                {
                    for (j = block_col; (j < block_col + 16) && (j < M); ++j)
                    {
                        if (i != j)
                        {
                            B[j][i] = A[i][j];
                        }
                        else
                        {
                            t1 = A[i][j];
                            t2 = i;
                        }
                    }
                    if (block_col == block_row)//diagonal
                    {
                        B[t2][t2] = t1;
                    }
                }
            }
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

char trans2_dec[] = "test2";
void trans2(int M, int N, int A[N][M], int B[M][N])
{
    int t1, t2, t3, t4, t5, t6, t7, t8;
    int i, j;
    int block_row, block_col;
    for (block_row = 0; block_row < N; block_row += 8)
    {
        for (block_col = 0; block_col < M; block_col += 8)
        {
            for (i = block_row; i < block_row + 4; i++)
            {
                j = block_col;
                t1 = A[i][j + 0];
                t2 = A[i][j + 1];
                t3 = A[i][j + 2];
                t4 = A[i][j + 3];
                t5 = A[i][j + 4];
                t6 = A[i][j + 5];
                t7 = A[i][j + 6];
                t8 = A[i][j + 7];

                B[j + 0][i] = t1;
                B[j + 1][i] = t2;
                B[j + 2][i] = t3;
                B[j + 3][i] = t4;

                B[j + 0][i + 4] = t5;
                B[j + 1][i + 4] = t6;
                B[j + 2][i + 4] = t7;
                B[j + 3][i + 4] = t8;
            }
            for (j = block_col; j < block_col + 4; j++)
            {
                i = block_row;
                t1 = A[i + 4][j];
                t2 = A[i + 5][j];
                t3 = A[i + 6][j];
                t4 = A[i + 7][j];

                t5 = B[j][i + 4];
                t6 = B[j][i + 5];
                t7 = B[j][i + 6];
                t8 = B[j][i + 7];

                B[j][i + 4] = t1;
                B[j][i + 5] = t2;
                B[j][i + 6] = t3;
                B[j][i + 7] = t4;

                B[j + 4][i + 0] = t5;
                B[j + 4][i + 1] = t6;
                B[j + 4][i + 2] = t7;
                B[j + 4][i + 3] = t8;
            }

            for (i = block_row + 4; i < block_row + 8; i += 2)
            {
                j = block_col + 4;
                t1 = A[i][j + 0];
                t2 = A[i][j + 1];
                t3 = A[i][j + 2];
                t4 = A[i][j + 3];

                t5 = A[i + 1][j + 0];
                t6 = A[i + 1][j + 1];
                t7 = A[i + 1][j + 2];
                t8 = A[i + 1][j + 3];

                B[j + 0][i] = t1;
                B[j + 1][i] = t2;
                B[j + 2][i] = t3;
                B[j + 3][i] = t4;

                B[j + 0][i + 1] = t5;
                B[j + 1][i + 1] = t6;
                B[j + 2][i + 1] = t7;
                B[j + 3][i + 1] = t8;
            }
        }
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);

    registerTransFunction(trans2, trans2_dec);
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;
    int flag = 1;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                printf("i: %d, j: %d\n", i, j);
                flag = 0;
                // return 0;
            }
        }
    }
    return flag;
    // return 1;
}
