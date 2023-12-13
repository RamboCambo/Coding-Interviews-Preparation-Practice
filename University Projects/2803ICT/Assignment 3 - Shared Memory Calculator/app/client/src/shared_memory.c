#include "shared_memory.h"

int access_shared_memory(int *fd1, int *fd2, void **syncronised_shared_mem, void **unsyncronised_shared_mem, sem_t **sem)
{
    // Access shared memory.
    *fd1 = shm_open(SYNCRONISED_SHARED_MEM_NAME, O_RDWR, 0666);
    if (*fd1 == -1) {
        return -1;
    }

    *syncronised_shared_mem = mmap(NULL, SYNCRONISED_SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, *fd1, 0);
    if (*syncronised_shared_mem == MAP_FAILED) {
        return -1;
    }

    *fd2 = shm_open(UNSYNCRONISED_SHARED_MEM_NAME, O_RDWR, 0666);
    if (*fd2 == -1) {
        munmap(*syncronised_shared_mem, SYNCRONISED_SHARED_MEM_SIZE); // Detach from shared memory.
        return -1;
    }

    *unsyncronised_shared_mem = mmap(NULL, UNSYNCRONISED_SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, *fd2, 0);
    if (*unsyncronised_shared_mem == MAP_FAILED) {
        munmap(*syncronised_shared_mem, SYNCRONISED_SHARED_MEM_SIZE); // Detach from shared memory.
        return -1;
    }

    // Access semaphore.
    *sem = sem_open(SEM_NAME, 0); // 0 indicates that the semaphore should already exist.
    if (*sem == SEM_FAILED) {
        munmap(syncronised_shared_mem, SYNCRONISED_SHARED_MEM_SIZE); // Detach from shared memory.
        munmap(unsyncronised_shared_mem, UNSYNCRONISED_SHARED_MEM_SIZE); // Detach from shared memory.
        return -1;
    }

    return 1;
}