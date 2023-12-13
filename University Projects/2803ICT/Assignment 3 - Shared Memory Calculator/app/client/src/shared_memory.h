#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

// Define the number of slots and server flags.
#define NUM_SLOTS 10

// Defining the structure of the syncronised shared memory segment.
typedef struct {
    uint32_t number;
    uint32_t slot[NUM_SLOTS];
    char clientflag;
    char serverflag[NUM_SLOTS];
} SyncronisedSharedFields;

// Define the structure of the unsyncronised shared memory segment.
typedef struct {
    char progress[NUM_SLOTS];
} UnsyncronisedSharedFields;

#define SYNCRONISED_SHARED_MEM_SIZE sizeof(SyncronisedSharedFields)
#define UNSYNCRONISED_SHARED_MEM_SIZE sizeof(UnsyncronisedSharedFields)

#define SEM_NAME "my_sem"
#define SYNCRONISED_SHARED_MEM_NAME "my_shm"
#define UNSYNCRONISED_SHARED_MEM_NAME "my_shm2"
#define SUCCESS 1
#define FAILURE 0

int access_shared_memory(int *fd1, int *fd2, void **syncronised_shared_mem, void **unsyncronised_shared_mem, sem_t **sem);