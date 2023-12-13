#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>

#define SUCCESS 1
#define FAILURE 0

int initialize_semaphore(const char* sem_name, sem_t **sem);
int cleanup_semaphore(sem_t *sem, const char* sem_name);

#endif
