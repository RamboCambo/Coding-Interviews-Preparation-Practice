#include <stdlib.h>

#include "server.h"

#define NUMBER_BITS 32

int fd1;
int fd2;
void *syncronised_shared_mem;
void *unsyncronised_shared_mem;
sem_t *sem;

uint32_t MAX_THREADS = 320;
uint32_t CURRENT_THREADS = 0;

SyncronisedSharedFields *synced_fields;
UnsyncronisedSharedFields *unsynced_fields;

// Create a pthread mutex to protect the unsyncronised shared memory.
pthread_mutex_t unsyncronised_shared_mem_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t current_threads_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

Queue *queue;


uint32_t find_completed_slot() {
    pthread_mutex_lock(&unsyncronised_shared_mem_mutex);
    for (int i = 0; i < NUM_SLOTS; i++) {
        if (unsynced_fields->progress[i] == 32) {
            pthread_mutex_unlock(&unsyncronised_shared_mem_mutex);

            return i;
        }
    }
    pthread_mutex_unlock(&unsyncronised_shared_mem_mutex);

    return NUM_SLOTS;
}

void send_back_factor(uint32_t slot, uint32_t factor) {
    int waiting_for_serverflag = 1;
    while(waiting_for_serverflag)
    {
        sem_wait(sem);
        waiting_for_serverflag = synced_fields->serverflag[slot];
        if(waiting_for_serverflag == 0)
        {
            synced_fields->slot[slot] = factor;
            synced_fields->serverflag[slot] = 1;
            sem_post(sem);

            break;
        }

        sem_post(sem);
        usleep(10000); // Sleep for 10ms
    }
}

void *rotate_and_factorise(void *arg) 
{
    // Cast the argument to the correct type.
    struct RotateAndFactoriseArgs *args = (struct RotateAndFactoriseArgs *)arg;

    // Rotate the number.
    uint32_t rotated_number = rotate_number_right(args->number, args->bits);

    // Factorise the number.
    if(rotated_number > 0)
    {
        while (rotated_number % 2 == 0)
        {
            send_back_factor(args->slot, 2);
            rotated_number /= 2;
        }
        uint32_t f = 3;
        while (f * f <= rotated_number)
        {
            if (rotated_number % f == 0)
            {
                send_back_factor(args->slot, f);
                rotated_number /= f;
            }
            else
            {
                f += 2;
            }
        }
        if (rotated_number != 1)
        {
            send_back_factor(args->slot, rotated_number);
        }
    }

    pthread_mutex_lock(&unsyncronised_shared_mem_mutex);
    unsynced_fields->progress[args->slot]++;
    pthread_mutex_unlock(&unsyncronised_shared_mem_mutex);

    // Decrement the current threads counter
    pthread_mutex_lock(&current_threads_mutex);
    CURRENT_THREADS--;
    pthread_mutex_unlock(&current_threads_mutex);

    free(args);

    return NULL;
}

void *queue_thread() {
    while(1) {
        // Check if there is a thread available to process the number
        pthread_mutex_lock(&current_threads_mutex);
        while(CURRENT_THREADS >= MAX_THREADS) {
            pthread_mutex_unlock(&current_threads_mutex);
            pthread_mutex_lock(&current_threads_mutex);
        }
        pthread_mutex_unlock(&current_threads_mutex);

        pthread_mutex_lock(&queue_mutex);
        struct RotateAndFactoriseArgs *args = dequeue(queue);
        pthread_mutex_unlock(&queue_mutex);

        if(args != NULL) {
            pthread_mutex_lock(&current_threads_mutex);
            CURRENT_THREADS++;
            pthread_mutex_unlock(&current_threads_mutex);

            pthread_t thread_id;
            pthread_create(&thread_id, NULL, rotate_and_factorise, args);
        }
    }
    return NULL;
}

int test_mode_check_if_no_requests() {
    int all_progresses_32 = 1;
    pthread_mutex_lock(&unsyncronised_shared_mem_mutex);
    for (int i = 0; i < NUM_SLOTS; i++)
    {
        if (unsynced_fields->progress[i] != 32)
        {
            all_progresses_32 = 0;
            break;
        }
    }
    pthread_mutex_unlock(&unsyncronised_shared_mem_mutex);

    return all_progresses_32;
}

void *handle_test()
{
    pthread_t thread_id = pthread_self();

    for(int i = 1; i <= 10; i++)
    {
        int waiting_for_serverflag = 1;
        while(waiting_for_serverflag)
        {
            sem_wait(sem);
            waiting_for_serverflag = synced_fields->serverflag[0];
            if(waiting_for_serverflag == 0)
            {
                // Just write the output to slot 0
                synced_fields->slot[0] = thread_id * 10 + i;
                synced_fields->serverflag[0] = 1;

                struct timespec ts;
                clock_gettime(CLOCK_REALTIME, &ts);
                srand(ts.tv_nsec);
                int random_number = rand() % 91 + 10;

                printf("Server sent %ld sleeping for %dms\n", thread_id * 10 + i, random_number);

                usleep(random_number);
                sem_post(sem);

                break;
            }

            sem_post(sem);
        }
    }

    return NULL;
}

// This function is called by the thread to handle incoming requests.
void *handle_requests()
{
    while (1)
    {
        sem_wait(sem);

        // Wait for the client to signal that it has finished writing to the unsyncronised shared memory.
        if(synced_fields->clientflag == 1)
        {
            // Find the first slot that is free.
            uint32_t slot = find_completed_slot();

            // Test Mode
            if(synced_fields->number == 0)
            {
                // Check if all progresses are 32
                int has_no_requests = test_mode_check_if_no_requests();

                slot = NUM_SLOTS;

                if(has_no_requests)
                {
                    slot = NUM_SLOTS + 1;

                    for(int i = 0; i < 3; i++)
                    {
                        pthread_t thread_id;
                        pthread_create(&thread_id, NULL, handle_test, NULL);
                    }                    
                }
            }
            else
            {
                // If slot is NUM_SLOTS, then all slots are full.
                if (slot == NUM_SLOTS)
                {
                    printf("Server: All slots are full\n");
                }
                else
                {
                    printf("Server: Found free slot %d\n", slot);
                    // Set the progress to zero
                    pthread_mutex_lock(&unsyncronised_shared_mem_mutex);
                    unsynced_fields->progress[slot] = 0;
                    pthread_mutex_unlock(&unsyncronised_shared_mem_mutex);

                    pthread_mutex_lock(&queue_mutex);
                    for (int i = 0; i < NUMBER_BITS; i++) {    
                        struct RotateAndFactoriseArgs *args = (struct RotateAndFactoriseArgs *)malloc(sizeof(struct RotateAndFactoriseArgs));
                        args->number = synced_fields->number;
                        args->bits = i;
                        args->slot = slot;

                        enqueue(queue, args);
                    }
                    pthread_mutex_unlock(&queue_mutex);
                }
            }

            synced_fields->clientflag = 0;
            // if slot is NUM_SLOTS, then all slots are full.
            synced_fields->number = slot;
        }

        sem_post(sem);
    }
}

int main(int argc, char *argv[])
{
    // Initialize syncronised shared memory.
    if (!initialize_shared_memory(SYNCRONISED_SHARED_MEM_NAME, &fd1, &syncronised_shared_mem))
    {
        perror("Failed to initialize shared memory");
        return -1;
    }

    // Initialize unsyncronised shared memory.
    if (!initialize_shared_memory(UNSYNCRONISED_SHARED_MEM_NAME, &fd2, &unsyncronised_shared_mem))
    {
        perror("Failed to initialize shared memory");
        cleanup_shared_memory(SYNCRONISED_SHARED_MEM_NAME, syncronised_shared_mem);
        return -1;
    }

    // Map the shared memory to our structure.
    synced_fields = (SyncronisedSharedFields *)syncronised_shared_mem;
    unsynced_fields = (UnsyncronisedSharedFields *)unsyncronised_shared_mem;

    // Initialize the entire structure to zero.
    memset(synced_fields, 0, SYNCRONISED_SHARED_MEM_SIZE);
    memset(unsynced_fields, 0, UNSYNCRONISED_SHARED_MEM_SIZE);

    // Set progress to 32 for all slots to indicate they're all 'done' or free to use for the first run
    for (int i = 0; i < NUM_SLOTS; i++)
    {
        unsynced_fields->progress[i] = 32;
    }

    // Initialize semaphore.
    if (!initialize_semaphore(SEM_NAME, &sem))
    {
        perror("Failed to initialize semaphore");
        cleanup_shared_memory(SYNCRONISED_SHARED_MEM_NAME, syncronised_shared_mem);
        cleanup_shared_memory(UNSYNCRONISED_SHARED_MEM_NAME, unsyncronised_shared_mem);
        return -1;
    }

    // Check if the user wants to change the number of threads from the command line argc
    if (argc > 1)
    {
        MAX_THREADS = atoi(argv[1]);
    }

    // Check if the number of threads is valid
    if (MAX_THREADS < 1 || MAX_THREADS > 320)
    {
        printf("Invalid number of threads! Must be >= 1 and <= 320!\n");
        cleanup_semaphore(sem, SEM_NAME);
        cleanup_shared_memory(SYNCRONISED_SHARED_MEM_NAME, syncronised_shared_mem);
        cleanup_shared_memory(UNSYNCRONISED_SHARED_MEM_NAME, unsyncronised_shared_mem);
        return -1;
    }

    queue = createQueue();

    pthread_t queue_thread_id;
    pthread_create(&queue_thread_id, NULL, queue_thread, NULL);

    // Create a thread to handle incoming requests
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, handle_requests, NULL);

    while(1)
    {
        char input; 
        int result = scanf("%c", &input);
        if (result > 0)
        { 
            if (input == 'q') {
                break;
            }
        }
    }

    // Cancel the thread
    pthread_cancel(thread_id);

    // Cleanup.
    cleanup_semaphore(sem, SEM_NAME);
    cleanup_shared_memory(SYNCRONISED_SHARED_MEM_NAME, syncronised_shared_mem);
    cleanup_shared_memory(UNSYNCRONISED_SHARED_MEM_NAME, unsyncronised_shared_mem);

    return 0;
}
