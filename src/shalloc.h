#ifndef SHALLOC_H_
#define SHALLOC_H_

#include <cstring>

bool init_shared_memory();
void destroy_shared_memory();

void *sh_malloc(size_t bytes);
void sh_free(void *ptr);

#endif // SHALLOC_H_
