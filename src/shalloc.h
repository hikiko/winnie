#ifndef SHALLOC_H_
#define SHALLOC_H_

bool init_shared_memory();
void destroy_shared_memory();

void *shmalloc(size_t bytes);
void shmfree(void *ptr);

#endif // SHALLOC_H_
