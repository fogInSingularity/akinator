#ifndef MURMURHASH_H
#define MURMURHASH_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include "../../libcode/include/debug.h"
#include "../../libcode/include/my_assert.h"

uint32_t Hash(const uint8_t* key, size_t len, uint32_t seed = 0);

#endif // MURMURHASH_H
