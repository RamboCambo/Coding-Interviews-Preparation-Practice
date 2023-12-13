#include <sys/time.h>
#include <stdint.h>
#include "time_utils.h"
#include <stddef.h>

uint64_t get_current_time_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}