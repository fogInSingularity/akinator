#include "../include/hash.h"

//static-----------------------------------------------------------------------

static inline uint32_t HashScramble(uint32_t k);

//-----------------------------------------------------------------------------

uint32_t Hash(const uint8_t* key, size_t len, uint32_t seed) {
  ASSERT(key != nullptr);

	uint32_t h = seed;
  uint32_t k = 0;

  for (size_t i = len >> 2; i != 0; i--) {
    memcpy(&k, key, sizeof(uint32_t));

    key += sizeof(uint32_t);

    h ^= HashScramble(k);
    h = (h << 13) | (h >> 19);
    h = h * 5 + 0xe6546b64;
  }

  k = 0;
  for (size_t i = len & 3; i != 0; i--) {
    k <<= 8;
    k |= key[i - 1];
  }

  h ^= HashScramble(k);

	h ^= (uint32_t)len;
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;

	return h;
}

//static-----------------------------------------------------------------------

static inline uint32_t HashScramble(uint32_t k) {
  k *= 0xcc9e2d51;
  k  = (k << 15) | (k >> 17);
  k *= 0x1b873593;

  return k;
}

