#include "semaphore.h"

int initialize_semaphore(const char* sem_name, sem_t **sem)
{
    *sem = sem_open(sem_name, O_CREAT | O_EXCL, 0666, 1);
    if (*sem == SEM_FAILED)
    {
        if (errno == EEXIST)
        {
            // If semaphore already exists, try to open it without O_CREAT | O_EXCL.
            *sem = sem_open(sem_name, 0);
            if (*sem == SEM_FAILED)
                return FAILURE;
        }
        else
        {
            return FAILURE;
        }
    }
    return SUCCESS;
}

int cleanup_semaphore(sem_t *sem, const char* sem_name)
{
    if (sem_close(sem) == -1)
        return FAILURE;

    // Unlink the semaphore name to ensure it's destroyed.
    if (sem_unlink(sem_name) == -1)
        return FAILURE;

    return SUCCESS;
}