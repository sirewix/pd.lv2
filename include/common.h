#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdint.h>

#define PUB

#ifdef DEBUG
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...)
#endif

typedef uint32_t uint;

extern void debug_float_array(const float* arr, uint size);
#endif
