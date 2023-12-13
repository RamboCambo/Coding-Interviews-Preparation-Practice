#include "shared_memory.h"

int initialize_shared_memory(const char *shm_name, int *fd, void **shared_mem)
{
    *fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (*fd == -1)
        return 0;

    if (ftruncate(*fd, SYNCRONISED_SHARED_MEM_SIZE) == -1)
        return 0;

    *shared_mem = mmap(NULL, SYNCRONISED_SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, *fd, 0);
    if (*shared_mem == MAP_FAILED)
        return 0;

    return 1;
}

int cleanup_shared_memory(const char *shm_name, void *shared_mem)
{
    if (munmap(shared_mem, SYNCRONISED_SHARED_MEM_SIZE) == -1)
        return 0;
    if (shm_unlink(shm_name) == -1)
        return 0;

    return 1;
}
