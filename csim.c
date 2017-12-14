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
	int evict;
}_state  = {0};

struct set
{ // live cache lines array size is unknown
  // live bit equals the number of assigned cachlines inside the set
  int live_bit;
  int set_id;
  int age_bit;
  // live_E points to an array live tags
  long long *live_E;
} s_default = {0};

struct mempool{
// it points to an array of sets, max number 1<<_s
  struct set * curr_s;
//  void * curr_s;
  struct set *live_s;
} cachpool = {0};

void parseArgs(int argc, char **argv);
int readline(int fd, char *buf, int maxlen);
void cache_once(char *line);
// checkcach returns 1 if request hit in the pool
// checkcach returns 0 if miss
void initcachpool(void);
int checkcachpool(long long request);
void freepool(void);

int main(int argc, char **argv)
{
    char line[MAXLINE];

    memset(line, 0, MAXLINE);
    parseArgs(argc, argv);
   
    initcachpool();
	
    while((readline(_state.fd, line, MAXLINE)) != 0)
    {
      cache_once(line);
      memset(line, 0, MAXLINE);
      fflush(stdout);
    }
    printSummary(_state.hit, _state.miss, _state.evict);
    fflush(stdout);
    freepool();
    fflush(stdout);
  
    if(close(_state.fd) != 0)
    {
	    fprintf(stderr, "close file error\n");
	    fflush(stderr);	    
	    exit(0);
    }
    return 0;
}

void cache_once(char *line)
{
	char *c = (char *)malloc(8);
	long long request;
	int status;
	memset(c, 0, 8);

	sscanf(line, "%s %llx,%*d", c, &request);
	if(!strcmp(c, "I"))	
	// we only cares about data cache performance
		return ;
	if(1==_state._verbose)
	  printf("%s 0x%llx: ", c, request);

	for(int i=0; i < 1 + (!strcmp(c, "M")); i++)
	{
	status = checkcachpool(request);
	switch(status)
	{
		case 1:
	// it hits
			_state.hit++;
			if(1==_state._verbose)
				printf("hit ");
			break;
		case 0:
			_state.miss++;
			if(1==_state._verbose)
				printf("miss ");
			break;
		case 2:
			_state.miss++;
			_state.evict++;
			if(1==_state._verbose)
				printf("miss eviction ");
			break;
	  }
	}
	printf("\n");
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
				fflush(stdout);
				exit(0);
			}
		}
	}
	_state._t = 64 - _state._s - _state._b;
}

void initcachpool(void)
{
  cachpool.live_s = (struct set*)malloc((1<<_state._s) * sizeof(struct set));
  cachpool.curr_s = (struct set*)cachpool.live_s;
 
  if(1==_state._verbose)
  {
    printf("Each set size%lu\nSet array size%lu\n",sizeof(struct set), (1<<_state._s) * sizeof(struct set));
    printf("Address of current set%p\nNext set%p\n", cachpool.live_s, cachpool.live_s + SET_SIZE);
    printf("Current pointer to the pool%p\n", cachpool.curr_s);
    fflush(stdout);
  }
}

int checkcachpool( long long request)
{
	// get s
	int s = request &(~(1<<31>>(_state._t-1))) & (1<<31>>(31 - _state._b));
	long long tag = request & (1<<31 >> (_state._t-1));
	struct set *ptr = cachpool.live_s;
	if(1==_state._verbose)
		printf("tag=%llx, set=%x: ",tag, s);

	
	// curr_s is always pointing to an empty slot 
	for(; ptr != cachpool.curr_s; ptr += SET_SIZE)
	{
	// looping through all the sets

		if(ptr->set_id == s)
		{
		// if found the set
		  for(int i = 0; i < ptr->live_bit; i++)
		  {
		// compare the tag
		if(1 == _state._verbose)
		{
			printf("slot value:%llx, ", *(ptr->live_E + i));
			printf("tag:%llx, ", tag);
			fflush(stdout);			
		}
		    if( tag == *(ptr->live_E + i ))
		    {
		// if found the tag, hit
			return 1;
		    }
		  }
		// set is live, tag is not

	// if live_bit == E; eviction happens
	//	printf("livebit=%x, _E=%x\n", ptr->live_bit, _state._E);
		if(ptr->live_bit == _state._E)
		{
			// age_bit
			*(ptr->live_E + (ptr->age_bit % _state._E)) = tag;
			ptr->age_bit++;
			return 2;
		}

		// else, add the tag to the new slot
		  *(ptr->live_E + ptr->live_bit) = tag;
		  if(1==_state._verbose)
		  {
			  printf("Add tag to the new slot in a live set\n");
			  printf("prev live_E ends at %p,new live_E ends at %p\n", (ptr->live_E + ptr->live_bit), (ptr->live_E + ptr->live_bit + 1));
		  }
		// increment live_bit
		  ptr->live_bit++;
		  return 0;
		}
	}
	// set is not live
	// increment curr_s
	
	cachpool.curr_s += SET_SIZE;
	// set set_id
	ptr->set_id = s;
	// init the live_E
	ptr->live_E = (long long *) malloc(_state._E * sizeof(long long));
	// add to the live_E
	*(ptr->live_E) = tag; 
	// set set live_bit = 1
	ptr->live_bit = 1;
	return 0;
}

void freepool(void)
{
	do{
		if(1 <= cachpool.curr_s->live_bit)
		{
			free(cachpool.curr_s->live_E);
		}
		cachpool.curr_s -= SET_SIZE;
	}while(cachpool.live_s < cachpool.curr_s);
	free(cachpool.live_s);
}
