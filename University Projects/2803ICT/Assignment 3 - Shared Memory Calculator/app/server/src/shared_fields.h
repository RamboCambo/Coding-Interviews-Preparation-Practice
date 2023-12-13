#ifndef SHARED_FIELDS_H
#define SHARED_FIELDS_H

#include <stdint.h>

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

#endif // SHARED_FIELDS_H