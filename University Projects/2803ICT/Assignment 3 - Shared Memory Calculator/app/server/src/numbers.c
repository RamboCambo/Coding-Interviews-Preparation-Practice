#include "numbers.h"

uint32_t rotate_number_right(uint32_t input, uint8_t bits)
{
    return (input >> bits) | (input << (32 - bits));
}

uint32_t *trial_division(uint32_t n, size_t *len)
{
    if(n == 0)
    {
        *len = 0;
        return NULL;
    }

    uint32_t *a = malloc(sizeof(uint32_t) * n);
    size_t i = 0;
    while (n % 2 == 0)
    {
        a[i++] = 2;
        n /= 2;
    }
    uint32_t f = 3;
    while (f * f <= n)
    {
        if (n % f == 0)
        {
            a[i++] = f;
            n /= f;
        }
        else
        {
            f += 2;
        }
    }
    if (n != 1)
    {
        a[i++] = n;
    }
    *len = i;
    return a;
}