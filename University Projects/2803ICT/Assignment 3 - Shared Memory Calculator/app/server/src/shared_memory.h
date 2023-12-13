#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include "shared_fields.h"

int initialize_shared_memory(const char *shm_name, int *fd, void **shared_mem);
int cleanup_shared_memory(const char *shm_name, void *shared_mem);

#endif
