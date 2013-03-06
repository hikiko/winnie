#include <map>
#include <stdlib.h>

#include "shalloc.h"

#define POOL_SIZE 16777216
#define BLOCK_SIZE 1024

static unsigned char *pool;
static std::map<unsigned char*, size_t> alloc_sizes;
//address size

bool init_shared_memory()
{
	if(!(pool = (unsigned char *)malloc(POOL_SIZE))) {
		return false;
	}
}

void destroy_shared_memory()
{
	//TODO print statistics
	free(pool);
}

// an zitaei 45 mb 8a vriskw posa blocks kai meta
// 8a psaxnw tosa sunexomena bits sto bitmap
// an den exei return 0
// pool[arxidi]
