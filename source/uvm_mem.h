#pragma once

/* **** */

#include <stddef.h>

/* **** */

int mem_ifetch(void* param, unsigned long *const rd, unsigned long *const ea, const size_t bytes);
int mem_read(void* param, unsigned long *const rd, const unsigned long ea, const size_t bytes);
int mem_write(void* param, unsigned const long rd, const unsigned long ea, const size_t bytes);
