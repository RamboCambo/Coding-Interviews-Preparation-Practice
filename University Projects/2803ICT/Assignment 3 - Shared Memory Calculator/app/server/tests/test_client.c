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

int main() {
    sleep(1); // Wait for the server to initialize.

    int fd1;
    int fd2;
    void *syncronised_shared_mem;
    void *unsyncronised_shared_mem;
    sem_t *sem;

    // Access shared memory.
    fd1 = shm_open(SYNCRONISED_SHARED_MEM_NAME, O_RDWR, 0666);
    if (fd1 == -1) {
        perror("Failed to open shared memory");
        return -1;
    }

    syncronised_shared_mem = mmap(NULL, SYNCRONISED_SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
    if (syncronised_shared_mem == MAP_FAILED) {
        perror("Failed to map shared memory");
        return -1;
    }

    fd2 = shm_open(UNSYNCRONISED_SHARED_MEM_NAME, O_RDWR, 0666);
    if (fd2 == -1) {
        munmap(syncronised_shared_mem, SYNCRONISED_SHARED_MEM_SIZE); // Detach from shared memory.
        perror("Failed to open shared memory");
        return -1;
    }

    unsyncronised_shared_mem = mmap(NULL, UNSYNCRONISED_SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
    if (unsyncronised_shared_mem == MAP_FAILED) {
        munmap(syncronised_shared_mem, SYNCRONISED_SHARED_MEM_SIZE); // Detach from shared memory.
        perror("Failed to map shared memory");
        return -1;
    }

    SyncronisedSharedFields *syncronised_fields = (SyncronisedSharedFields *)syncronised_shared_mem;
    UnsyncronisedSharedFields *unsyncronised_fields = (UnsyncronisedSharedFields *)unsyncronised_shared_mem;

    // Print values from the unsyncronised shared memory.
    for(int i = 0; i < NUM_SLOTS; i++) {
        printf("Progress[%d]: %d\n", i, unsyncronised_fields->progress[i]);
    }

    // Access semaphore.
    sem = sem_open(SEM_NAME, 0); // 0 indicates that the semaphore should already exist.
    if (sem == SEM_FAILED) {
        munmap(syncronised_shared_mem, SYNCRONISED_SHARED_MEM_SIZE); // Detach from shared memory.
        munmap(unsyncronised_shared_mem, UNSYNCRONISED_SHARED_MEM_SIZE); // Detach from shared memory.
        perror("Failed to open semaphore");
        return -1;
    }

    // Wait for the server to finish writing.
    sem_wait(sem);

    // Print values from the shared memory.
    printf("Number: %u\n", syncronised_fields->number);
    for (int i = 0; i < 10; i++) {
        printf("Slot[%d]: %u\n", i, syncronised_fields->slot[i]);
        printf("Serverflag[%d]: %c\n", i, syncronised_fields->serverflag[i]);
    }
    printf("Clientflag: %c\n", syncronised_fields->clientflag);

    // Signal to the server that we're done reading.
    sem_post(sem);

    // Cleanup.
    munmap(syncronised_shared_mem, SYNCRONISED_SHARED_MEM_SIZE); // Detach from shared memory.
    munmap(unsyncronised_shared_mem, UNSYNCRONISED_SHARED_MEM_SIZE); // Detach from shared memory.
    sem_close(sem); // Close semaphore.

    return 0;
}
