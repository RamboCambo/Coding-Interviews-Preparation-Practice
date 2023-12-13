#ifndef CONSTANTS_SERVER_H
#define CONSTANTS_SERVER_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "shared_fields.h"
#include "shared_memory.h"
#include "semaphore.h"
#include "shared_fields.h"
#include "numbers.h"
#include "queue.h"

#define SEM_NAME "my_sem"
#define SYNCRONISED_SHARED_MEM_NAME "my_shm"
#define UNSYNCRONISED_SHARED_MEM_NAME "my_shm2"

#endif