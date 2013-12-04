#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define _ADDR_DEBUG
#define _FIFO_DEBUG
#define _LRU_DEBUG

#define BIT_IN_ADDR 16
#define MEM_SIZE	16
#define PAGE_SIZE	1

typedef struct page {
	int page_num;
	int recently_used;
} PAGE;

int* genAddr(int naddr, int nseed);
int fifo(int* arr, int size);
int lru(int* arr, int size);
int optimal(int* arr, int size);

bool isexist(PAGE* arr, int size, int value);
int getPageNumber(int);
long unsigned int dectobin(long unsigned int);
int bintodec(long unsigned int);
	
int main(int argc, char** argv) {
    int i;	
    int *arr;
    int access_mem_count;
    int seed;

    if(argc<3) {
    	fprintf(stderr,"usage: %s [access_mem_count] [seed]\n",argv[0]);
    	exit(-1);
    }

    access_mem_count=atoi(argv[1]);
    seed=atoi(argv[2]);

    if(access_mem_count<100||access_mem_count>1000000) {
    	fprintf(stderr,"First parameter is wrong range\n");
    	exit(-1);
    }

    arr=(int*)malloc(sizeof(int)*access_mem_count);

	arr = genAddr(access_mem_count, seed);
        
    printf("FIFO: %d\n",fifo(arr,access_mem_count));
    printf("LRU: %d\n",lru(arr,access_mem_count));
    /*
    for(i = 0; i < access_mem_count; i++)
		printf("%dst addr value : %d\n", i, arr[i]);
	*/
	return 0;
}

int* genAddr(int naddr, int nseed) {
	int i;
	int tmp1, tmp2;
	int *addr_arr = malloc(naddr * sizeof(int));

	if(addr_arr == NULL)
	{
		fprintf(stderr, "malloc is failed");
		exit(0);
	}

	// seed at least 50 addresses
	if( naddr < 50)
		return NULL;
	
	srand(nseed);
	for(i = 0; i < 50; i++)
		addr_arr[i] = rand() % ( 1 << BIT_IN_ADDR);

	for( i = 50; i < naddr; i++)
	{
		addr_arr[i] =0;
		tmp1 = rand() % 10;
		tmp2 = rand() % 50;
		
		if( tmp1 < 5)
		{
			addr_arr[i] = addr_arr[i-tmp1];
		}else if( tmp2 < 25)
		{
			addr_arr[i] = addr_arr[i-tmp2];
		}else
			addr_arr[i] = rand() % ( 1 << BIT_IN_ADDR);
	}
	return addr_arr;
}

bool isexist(PAGE* arr, int size, int value) {
	int i=0;
	for(; i<size; i++) {
		if(arr[i].page_num==value) {
			return true;
		}
	}

	return false;
}

int fifo(int* arr, int size) {
	int pages_size=MEM_SIZE/PAGE_SIZE;
	PAGE* pages=(PAGE*)malloc(sizeof(PAGE)*pages_size);
	int fault_count=0;
	int victim_frame=0;

	memset(pages,-1,sizeof(PAGE)*pages_size);
	int i;
	for(i=0; i<size; i++) {
		int frame_num=arr[i]/1024;
#ifdef ADDR_DEBUG
		fprintf(stdout,"Logical Address: %i\n",arr[i]);
		fprintf(stdout,"Frame number: %d\n",getPageNumber(arr[i]));
		fprintf(stdout,"Before frame number: %d\n",frame_num);
#endif
		if(!isexist(pages, pages_size, frame_num)) {
			fault_count++;

			pages[victim_frame].page_num=frame_num;
			pages[victim_frame].recently_used=0;
			victim_frame++;

			if(victim_frame>pages_size) {
				victim_frame=0;
			}
		}
	}

	free(pages);

	return fault_count;
}

int lru(int* arr, int size) {
	int pages_size=MEM_SIZE/PAGE_SIZE;
	PAGE* pages=(PAGE*)malloc(sizeof(PAGE)*pages_size);
	int fault_count=0;
	int victim_frame=0;

	memset(pages,-1,sizeof(PAGE)*pages_size);
	int i;
	for(i=0; i<size; i++) {
		int frame_num=arr[i]/1024;

#ifdef LRU_DEBUG
	fprintf(stdout,"[LRU] Page %d required\n",frame_num);
#endif

		if(!isexist(pages, pages_size, frame_num)) {
#ifdef LRU_DEBUG
	fprintf(stdout,"[LRU] Page %d does not exists\n",frame_num);
#endif
			fault_count++;
			
			int j;
			int high_value=0;
			for(j=0; j<pages_size; j++) {
				int temp=i-pages[j].recently_used;
#ifdef LRU_DEBUG
	fprintf(stdout,"[LRU] Page %d unused time %d\n",pages[j].page_num,temp);
#endif
				if(temp>high_value) {
					victim_frame=j;
					high_value=temp;
				}
			}
#ifdef LRU_DEBUG
	fprintf(stdout,"[LRU] Victim page %d will be changed with page %d\n",pages[victim_frame].page_num, frame_num);
#endif

			pages[victim_frame].page_num=frame_num;
			pages[victim_frame].recently_used=i;
		}else {
#ifdef LRU_DEBUG
	fprintf(stdout,"[LRU] Page %d exists\n",frame_num);
#endif
			int j;
			for(j=0; j<size; j++) {
				if(pages[j].page_num==frame_num) {
					pages[j].recently_used=i;
					break;
				}
			}
		}
	}

	free(pages);

	return fault_count;
}

int optimal(int* arr, int size) {

}

/* unused function
int getPageNumber(int address) {
	long unsigned int binary_addr=dectobin(address);
#ifdef ADDR_DEBUG
	fprintf(stdout,"Origin : %lu\n",binary_addr);
#endif
	binary_addr/=10000000000;
#ifdef ADDR_DEBUG
	fprintf(stdout,"Bit Shift : %lu\n",binary_addr);
#endif
	return bintodec(binary_addr);
}

long unsigned int dectobin(long unsigned int value) {
	if(value==0) return 0;
	if(value==1) return 1;
	return (value%2)+10*dectobin(value/2);
}

int bintodec(long unsigned int value) {
	if(value==0) return 0;
	int temp=value%10;
	temp+=2*bintodec(value/10);
	return temp;
}
*/