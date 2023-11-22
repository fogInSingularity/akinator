#include "../include/recalloc.h"

void* Recalloc(void* ptr, size_t cap, size_t used_size) {
  ASSERT(ptr != nullptr);

  ptr = realloc(ptr, cap);
  if (ptr == nullptr) { return ptr; }

  memset((Byte*)ptr + used_size, 0, cap - used_size);

  return ptr;
}
