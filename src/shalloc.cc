#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <map>

#include "shalloc.h"

#define SHMNAME	"/winnie.shm"

#define POOL_SIZE 16777216
#define BLOCK_SIZE 512

#define NUM_BLOCKS (POOL_SIZE / BLOCK_SIZE)
#define BITMAP_SIZE (NUM_BLOCKS / 32)

static bool is_allocated(int block_number);
static int addr_to_block(unsigned char *addr);
static unsigned char *block_to_addr(int block_number);
static void alloc_blocks(int block_pos, int num_blocks);
static void free_blocks(int block_pos, int num_blocks);

static void print_stats();
static int fd;

static unsigned char *pool;
static std::map<int, int> alloc_sizes; //starting block -> number of blocks

// 0 means not allocated 1 means allocated
static uint32_t bitmap[BITMAP_SIZE];

struct Statistics {
	int alloc_num;
	int free_num;
	int alloc_memsize;
	int free_memsize;
};

static Statistics stats;

bool init_shared_memory()
{
	if(((fd = shm_open(SHMNAME, O_RDWR | O_CREAT, S_IRWXU)) == -1)) {
		fprintf(stderr, "Failed to open shared memory: %s\n", strerror(errno));
		return false;
	}
	ftruncate(fd, POOL_SIZE);

	if((pool = (unsigned char*)mmap(0, POOL_SIZE, PROT_READ | PROT_WRITE,
					MAP_SHARED, fd, 0)) == (void*)-1) {
		fprintf(stderr, "Failed to map shared memory: %s\n", strerror(errno));
	}

	shm_unlink(SHMNAME);

	for(int i=0; i<BITMAP_SIZE; i++) {
		bitmap[i] = 0;
	}

	alloc_sizes.clear();
	memset(&stats, 0, sizeof stats);

	return true;
}

void destroy_shared_memory()
{
	print_stats();
	if(munmap(pool, POOL_SIZE) == -1) {
		fprintf(stderr, "Failed to unmap shared memory: %s\n", strerror(errno));
	}
}

void *sh_malloc(size_t bytes)
{
	if(!bytes) {
		return 0;
	}

	int num_blocks = (bytes + BLOCK_SIZE - 1) / BLOCK_SIZE;
	
	int free_block;
	int ctr = 0;
	for(int i=0; i<NUM_BLOCKS; i++) {
		if(!is_allocated(i)) {
			if(!ctr) {
				free_block = i;
			}
			ctr++;
		}
		else {
			ctr = 0;
		}

		if(ctr == num_blocks) {
			alloc_blocks(free_block, num_blocks);
			return block_to_addr(free_block);
		}
	}

	return 0;
}

void sh_free(void *ptr)
{
	int block = addr_to_block((unsigned char*)ptr);
	std::map<int, int>::iterator it;
	if((it = alloc_sizes.find(block)) != alloc_sizes.end()) {
		int num_blocks = it->second;
		free_blocks(block, num_blocks);
		alloc_sizes.erase(it);
	}
	else {
		fprintf(stderr, "Attempt to free non-existent blocks from: %d\n", block);
	}
}

static bool is_allocated(int block_number)
{
	int idx = block_number / 32;
	int bit_num = block_number % 32;

	if((bitmap[idx] >> bit_num) & 1) {
		return true;
	}

	return false;
}

static int addr_to_block(unsigned char *addr)
{
	assert(addr >= pool);
	assert(addr < pool + POOL_SIZE);

	return (addr - pool) / BLOCK_SIZE;
}

static unsigned char *block_to_addr(int block_number)
{
	assert(block_number >= 0);
	assert(block_number < NUM_BLOCKS);

	return pool + block_number * BLOCK_SIZE;
}

static void alloc_blocks(int block_pos, int num_blocks)
{
	for(int i=0; i<num_blocks; i++) {
		int block_number = i + block_pos;
		int idx = block_number / 32;
		int bit_num = block_number % 32;
	
		bitmap[idx] |= ((uint32_t)1 << bit_num); // or pow(2, i)
	}

	alloc_sizes[block_pos] = num_blocks;

	stats.alloc_num++;
	stats.alloc_memsize += BLOCK_SIZE * num_blocks;
}

static void free_blocks(int block_pos, int num_blocks)
{
	for(int i=0; i<num_blocks; i++) {
		int block_number = i + block_pos;
		int idx = block_number / 32;
		int bit_num = block_number % 32;

		bitmap[idx] &= ~((uint32_t)1 << bit_num);
	}

	stats.free_num++;
	stats.free_memsize += BLOCK_SIZE * num_blocks;
}

static void print_stats()
{
	printf("Total allocated memory: %d\n", stats.alloc_memsize);
	printf("Total deallocated memory: %d\n", stats.free_memsize);
	printf("Number of allocations: %d\n", stats.alloc_num);
	printf("Number of deallocations: %d\n", stats.free_num);
}

