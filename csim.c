#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
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
printf("do something");

}

void initMatrix(int M, int N, int A[N][M], int B[M][N])
{
printf("do something");

}

void correctTrans(int M, int N, int A[N][M], int B[M][N])
{
printf("do something");

}

void registerTransFunction(void (*trans)(int M,int N,int[N][M],int[M][N]), char* desc){
printf("do something");


}

void parseArgs(int argc, char **argv)
{
	char c;
	while((c = getopt_long(argc, argv, "hvs:E:b:t:", NULL,NULL)) != EOF)
	{
		switch(c){
			case 'h':
			{
				char *response = "Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\nOptions:\n \
	 -h         Print this help message.\n \
	 -v         Optional verbose flag.\n \
	 -s <num>   Number of set index bits.\n \
	 -E <num>   Number of lines per set.\n \
	 -b <num>   Number of block offset bits.\n \
	 -t <file>  Trace file.\n\n \
Examples:\n \
   linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n \
   linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace";
				printf("%s", response);
			}
			case 'v':
			case 's':
			case 'E':
			case 'b':
			case 't':
			break;
			default:
				printf("default");
		}
	}
}

