#include <stdio.h>
#include <stdlib.h>
#include "cachelab.h"

/*
 * **********************************************
 * t bits    |     s      |      b      |
 * *********************************************
 */

int s = 0;	// set index bits
int E = 0;	// lines per set
int b = 0;	// number of block offset bits
trans_func_t request;

void parseArgs(int argc, char **argv);

int main(int argc, char **argv)
{
    parseArgs(argc, argv);


    printSummary(0, 0, 0);
    return 0;
}

void printSummary(int hits, int misses, int evictions)
{


}

void initMatrix(int M, int N, int A[N][M], int B[M][N])
{

}

void correctTrans(int M, int N, int A[N][M], int B[M][N])
{

}

void registerTransFunction(void (*trans)(int M,int N,int[N][M],int[M][N]), char* desc)
{


}

void parseArgs(int argc, char **argv)
{

case

	-h 
	printf
	help

	-v
	verbose display trace info

	-s  <s>
	number of set index bits. S =2^s
	-E <E>
	-b <b>
	-t <tracefile>


}

