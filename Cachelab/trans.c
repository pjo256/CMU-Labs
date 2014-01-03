/* Philip J. Ottesen - NYUID: pjo256 
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
	/*
	if (M == N) {
		trans_square(M, N, A, B);
	} else {
		trans_diff(M, N, A, B);
	}
	*/
	int i, j, row_block, col_block;
	int diag = 0;
	int temp = 0;

	/*
	The access pattern for the defined problem sizes incorporate blocking; we define a sub-matrix of the matrix A with so	 me size b to be a square block. The outer-loops iterate across these block structures, with the two inner loops		  iterating through each block.
	*/

	if (N == 32) 
	{
		//Finding optimal block sizes included some guess-work; increased B-size for larger M & N when M == N proved less 		 //efficient. However, when M != N, i.e. rectangular matrix, larger B was the way to go.
		for (col_block = 0; col_block < N; col_block += 8) 
		{
			for (row_block = 0; row_block < N; row_block += 8) 
			{
			
				for (i = row_block; i < row_block + 8; i ++) 
				{

					for (j = col_block; j < col_block + 8; j ++) 
					{
						if (i != j) 
						{
							B[j][i] = A[i][j];
						}
						 else {
							//Reduce misses m < i*j in B by storing in temp instead of missing in B[j][i]
							temp = A[i][j];
							diag = i;
						}
					}
					
					//Transpose of a square-matrix has a unique property; no need to move elements on the diagonal. 
				
					if (row_block == col_block) 
					{
						//Misses in B reduced to m < i
						B[diag][diag] = temp;
					}
				}	
				
			}
		}

	} 

	else if (N == 64) 
	{

		//Iterate through matrix using column-major iteration over blocks
		for (col_block = 0; col_block < N; col_block += 4) 
		{
			for (row_block = 0; row_block < N; row_block += 4) 
			{
				//Iterate over each row using row-major iteration			
				for (i = row_block; i < row_block + 4; i ++) 
				{
					for (j = col_block; j < col_block + 4; j ++) 
					{
						if (i != j) 
						{
							B[j][i] = A[i][j];
						}
						else {
							//On the diagonal
							temp = A[i][j];
							diag = i;
						}
					}

					if (row_block == col_block) 
					{
						B[diag][diag] = temp;
					}
				}	
				
			}
		}
		

	} 
	else {
		
		//Iterate through matrix using column-major iteration over blocks
		for (col_block = 0; col_block < M; col_block += 16) 
		{
			for (row_block = 0; row_block < N; row_block += 16) 
			{		
				//Since our sizes are prime, not all blocks will be square sub-matrices
				//Consider corner-case when (row_block + 16 > N) => invalid access. Explicit check for i, j < n, m
				for (i = row_block; (i < row_block + 16) && (i < N); i ++) 
				{
					for (j = col_block; (j < col_block + 16) && (j < M); j ++) 
					{
					
						if (i != j) 
						{
							B[j][i] = A[i][j];
						}
						else 
						{
							temp = A[i][j];
							diag = i;
						}
					}

					if (row_block == col_block) {
						B[diag][diag] = temp;
					}

				}
			
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

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

