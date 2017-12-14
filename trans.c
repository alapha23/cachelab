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
    int i, j, k, c, d;
    int tmp;
    if (M <=32)
    {
// cacheline 32 bytes
// 5 sets
/*	B[0][0] = A[0][0] ;
	B[1][0] = A[0][1];
	B[2][0] = A[0][2];
	B[3][0] = A[0][3];

	B[0][1] = A[1][0] ;
	B[1][1] = A[1][1];
	B[2][1] = A[1][2];
	B[3][1] = A[1][3];

	B[0][2] = A[2][0] ;
	B[1][2] = A[2][1];
	B[2][2] = A[2][2];
	B[3][2] = A[2][3];
	
	B[0][31] = A[31][0] ;
	B[1][31] = A[31][1];
	B[2][31] = A[31][2];
	B[3][31] = A[31][3];
	
	*/
	for(k = 0; k <32; k +=8)
	for(i=0; i < 32; i++)
	{
	  for(j=0; j <8; j++)
	  {
	    tmp= A[i][j+k];
	    B[j+k][i]=tmp; 
	  }

	}
    }
    else if (M == 64)
    {
	    int colRun, rowRun, a0, a1, a2, a3, a4, a5, a6, a7; // supporting variables

    // two loops to go through each row and column block
    for(colRun=0; colRun<64; colRun+=8 ){
       	for(rowRun=0; rowRun<64; rowRun+=8 ){

        	for(k=0; k<4; k++){
        		a0 = A[colRun+k][rowRun+0];
        		a1 = A[colRun+k][rowRun+1];
        		a2 = A[colRun+k][rowRun+2];
        		a3 = A[colRun+k][rowRun+3];
        		a4 = A[colRun+k][rowRun+4];
        		a5 = A[colRun+k][rowRun+5];
        		a6 = A[colRun+k][rowRun+6];
        		a7 = A[colRun+k][rowRun+7];

        		B[rowRun+0][colRun+k+0] = a0;   // good job
        		B[rowRun+0][colRun+k+4] = a5;	
        		B[rowRun+1][colRun+k+0] = a1;	// good job
        		B[rowRun+1][colRun+k+4] = a6;	
        		B[rowRun+2][colRun+k+0] = a2;	// good job
        		B[rowRun+2][colRun+k+4] = a7;	
        		B[rowRun+3][colRun+k+0] = a3;	// good job
        		B[rowRun+3][colRun+k+4] = a4;	
        	}

            /* Part B, moving sub-matrix b to sub-matrix c
             * and moving A->B for sub-matrix b and move matrix d
             */
            /*
            Now that we have dealt with the first 4 col 8 arrow of A. The next job to deal with the "later use" assignment above. The "later use" assignments that we did above have taken a lot of places, so we need to bring these elements to their right positions.
             */
        	a0 = A[colRun+4][rowRun+4];
        	a1 = A[colRun+5][rowRun+4];
        	a2 = A[colRun+6][rowRun+4];
        	a3 = A[colRun+7][rowRun+4];
        	a4 = A[colRun+4][rowRun+3];
        	a5 = A[colRun+5][rowRun+3];
        	a6 = A[colRun+6][rowRun+3];
        	a7 = A[colRun+7][rowRun+3];

        	B[rowRun+4][colRun+0] = B[rowRun+3][colRun+4];  // B[4][0] = a4 = A[0][4] For example
        	B[rowRun+4][colRun+4] = a0;  // B[4][4] = A[4][4] For example
        	B[rowRun+3][colRun+4] = a4;
        	B[rowRun+4][colRun+1] = B[rowRun+3][colRun+5];
        	B[rowRun+4][colRun+5] = a1;
        	B[rowRun+3][colRun+5] = a5;
        	B[rowRun+4][colRun+2] = B[rowRun+3][colRun+6];
        	B[rowRun+4][colRun+6] = a2;
        	B[rowRun+3][colRun+6] = a6;
        	B[rowRun+4][colRun+3] = B[rowRun+3][colRun+7];
        	B[rowRun+4][colRun+7] = a3;
        	B[rowRun+3][colRun+7] = a7;

        	// this loops deal with the the remaning elements .
        	for(k=0;k<3;k++){
        		a0 = A[colRun+4][rowRun+5+k];
        		a1 = A[colRun+5][rowRun+5+k];
        		a2 = A[colRun+6][rowRun+5+k];
        		a3 = A[colRun+7][rowRun+5+k];
        		a4 = A[colRun+4][rowRun+k];
        		a5 = A[colRun+5][rowRun+k];
        		a6 = A[colRun+6][rowRun+k];
        		a7 = A[colRun+7][rowRun+k];

        		B[rowRun+5+k][colRun+0] = B[rowRun+k][colRun+4];
        		B[rowRun+5+k][colRun+4] = a0;
        		B[rowRun+k][colRun+4] = a4;
        		B[rowRun+5+k][colRun+1] = B[rowRun+k][colRun+5];
        		B[rowRun+5+k][colRun+5] = a1;
        		B[rowRun+k][colRun+5] = a5;
        		B[rowRun+5+k][colRun+2] = B[rowRun+k][colRun+6];
        		B[rowRun+5+k][colRun+6] = a2;
        		B[rowRun+k][colRun+6] = a6;
        		B[rowRun+5+k][colRun+3] = B[rowRun+k][colRun+7];
        		B[rowRun+5+k][colRun+7] = a3;
        		B[rowRun+k][colRun+7] = a7;
        	}
		}
   	 }
   }
    else{
		
		for (j = 0; j < M; j += 16)
		{
			for (i = 0; i < N; i += 16)
			{	
				/*not all blocks will be square, so (rowBlock + 16 > N) may get an invalid access. Therefore we need to regularly check for r<N and c<M */
				for(k = i; (k < N) && (k < i + 16); k++)
				{
					for(c = j; (c < M) && (c < j + 16); c++)
					{
						//row and column are not same
						if (k != c)
						{
							B[c][k] = A[k][c];
						}
						
						//row and column same. process like 32 x 32 
						else
						{
							tmp = A[k][c];
							d = k;
						}
					}
					
					//Diagonal
					if(i == j) 
					{
						B[d][d] = tmp;
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

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
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
 //   registerTransFunction(trans, trans_desc); 

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

