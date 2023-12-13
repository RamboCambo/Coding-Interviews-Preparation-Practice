#ifndef NUMBERS_H
#define NUMBERS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint32_t rotate_number_right(uint32_t input, uint8_t bits);
uint32_t *trial_division(uint32_t n, size_t *len);

#endif