#ifndef LIB_UTILS_H_
#define LIB_UTILS_H_

#include <stddef.h>
#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#include "lib_config.h"

#include "my_assert.h"

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif // #ifndef MIN
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif // #ifndef MAX

#define USE_VAR(var) (void)var;

static const double kEpsilon = 0.00000001;
#define IS_EQUAL(a, b) (abs(a - b) < kEpsilon)

void SwapBytes(void* a, void* b, size_t size);
void FillBytes(void* dest, const void* src,
               size_t n_elem, size_t size_elem);

#endif // LIB_UTILS_H_
