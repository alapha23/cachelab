#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include "cachelab.h"

#define MAXLINE 1024
#define SET_SIZE sizeof(struct set)
#define DEBUG_ 0 
/*
 * **********************************************
 * t bits    |     s      |      b      |
 * *********************************************
 */

struct csim{
	int _s;	// set index bits
	int _E;	// lines per set
	int _b;	// number of block offset bits
	int _t; // tag bits
	int _verbose;
	int fd; // descriptor of the trace file
	int hit;
	int miss;
	int eviction;
}_state;

struct set
{ // live cache lines array size is unknown
  int live_bit;
  int set_id;
  // live_E points to an array live tags
  int *live_E;
} s_default = {0};

struct mempool{
// it points to an array of sets, max number 1<<_s
//  struct set * curr_s;
  void * curr_s;
  struct set *live_s;
} cachpool;

void parseArgs(int argc, char **argv);
int readline(int fd, char *buf, int maxlen);
void cache_once(char *line);
// checkcach returns 1 if request hit in the pool
// get s --> compare tags with all E cachelines in the set
void initcachpool(void);
int checkcachpool(char* c, int request);
int add2cachpool(int request);
void freepool(void);

int main(int argc, char **argv)
{
    char line[MAXLINE];

    memset(line, 0, MAXLINE);
    parseArgs(argc, argv);
   
    initcachpool();
	
    while((readline(_state.fd, line, MAXLINE)) != 0)
    {
#if DEBUG_ == 1
      printf("%s", line);
      fflush(stdout);
#endif
      cache_once(line);
      memset(line, 0, MAXLINE);
    }

    freepool();
    printSummary(0, 0, 0);
    if(close(_state.fd) != 0)
    {
	    fprintf(stderr, "close file error\n");
	    fflush(stderr);	    
    }
    return 0;
}

void cache_once(char *line)
{
	char *c = (char *)malloc(8);
	memset(c, 0, 8);
	int request;
	sscanf(line, "%s %d,%*d", c, &request);
	if(!strcmp(c, "I"))	
	// we only cares about data cache performance
		return ;
if(!checkcachpool(c, request))
	  add2cachpool(request);


	if(1==_state._verbose)
	  printf("%s %d\n", c, request);


}

int readline(int fd, char *buf, int maxlen)
{
  char *c = (char*)malloc(maxlen);  
  int n;
  int status;

  memset(c, 0, maxlen);

  for(n=0; n<maxlen; n++)
  {
    status = read(fd, (c+n), 1);
#if DEBUG_ == 1
    FILE *ffd = fopen("./test.txt", "a");
    fprintf(ffd,  "%x", (int)*(c+n));
    fclose(ffd);
#endif

    if(status == 0)
    {
    // EOF
	strcat(buf, c);
	return 0;
    }
    else if( status == -1)
    {
	    fprintf(stderr, "readline error\n");
	    fflush(stderr);
	    exit(0);
    }

    if(*(c+n) == '\n')
    {	
	strcat(buf, c);
      return n+1; 
    }
  }

    fprintf(stderr, "readline length out of bound\n");
    fflush(stderr);
    exit(0);
}

void parseArgs(int argc, char **argv)
{
	char c;
	while((c = getopt_long(argc, argv, "hvs:E:b:t:", NULL,NULL)) != EOF)
	{
		switch(c){
		case 'v':
				_state._verbose = 1;
				break;
			case 's':
				_state._s = atoi(optarg);
				if( _state._s < 0 || _state._s > 32)
				{
					fprintf(stderr, "Set bit s=%d invalid\n", _state._s);
					fflush(stderr);
					exit(0);
				}
				break;
			case 'E':
				_state._E = atoi(optarg);
				if( _state._E < 0 || _state._E > 32)
				{
					fprintf(stderr, "Cacheline bit E=%d invalid\n", _state._E);
					fflush(stderr);
					exit(0);
				}
				break;
			case 'b':
				_state._b = atoi(optarg);
				if( _state._b < 0 || _state._b > 32)
				{
					fprintf(stderr, "Blocksize bit b=%d invalid\n", _state._b);
					fflush(stderr);
					exit(0);
				}
				break;			
			case 't':
				_state.fd = open(optarg, O_RDONLY, 0);
				if(_state.fd == -1)
				{
					fprintf(stderr, "%s: No such file or directory\n\n", optarg);
					fflush(stderr);
					exit(0);

				}
				break;
			case 'h':

			default:
			{
				char *response = "Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\nOptions:\n \
	 -h         Print this help message.\n \
	 -v         Optional verbose flag.\n \
	 -s <num>   Number of set index bits.\n \
	 -E <num>   Number of lines per set.\n \
	 -b <num>   Number of block offset bits.\n \
	 -t <file>  Trace file.\n\n\
Examples:\n\
   linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n\
   linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n";
				printf("%s", response);
			}
		}
	}
	_state._t = 64 - _state._s - _state._b;
}

void initcachpool(void)
{
  cachpool.live_s = (struct set*)malloc((1<<_state._s) * sizeof(struct set));
  cachpool.curr_s = (void*)cachpool.live_s;
 

  if(1==_state._verbose)
  {
    printf("Each set size%lu\nSet array size%lu\n",sizeof(struct set), (1<<_state._s) * sizeof(struct set));
    printf("Address of current set%p\nNext set%p\n", cachpool.live_s, cachpool.live_s + SET_SIZE);
    printf("Current pointer to the pool%p\n", cachpool.curr_s);
    fflush(stdout);
  }
}

int checkcachpool(char *c, int request)
{
	return 1;
}
int add2cachpool(int request)
{
	// get s
	int s = (request >> _state._b) & ~(0x80000000 >> (64 - _state._b));
s++;
	return 1;
}

void freepool(void)
{
	do{
	  if(1 == cachpool.curr_s->live_bit)
	    free(cachpool.curr_s->live_E);
	  cachpool.curr_s += SET_SIZE;
	}while(cachpool.live_s != cachpool.curr_s);
	free(cachpool.live_s);
}
