#include "../include/stack.h"

static const ssize_t MultiplyConst = 2;
static const size_t Nullifier = 0xDEAD;

#ifdef _CANARY_PROT
static const canary_t Canary = 0xDEADFA11;
#endif // _CANARY_PROT

//---------------------------------------------------------

static StackError StackResizeUp(Stack* stk);

static StackError StackResizeDown(Stack* stk);

static StackError StackRecalloc(Stack* stk);

static StackError StackOkData(Stack* stk);

#ifdef _HASH_PROT
static StackError StackHashUpdate(Stack* stk);
#endif // _HASH_PROT

#ifdef _CANARY_PROT
static canary_t* AdrLCanary(Stack* stk);
static canary_t* AdrRCanary(Stack* stk);
#endif // _CANARY_PROT

static Elem* AdrDataElem(Stack* stk, ssize_t index);

//---------------------------------------------------------

StackError StackCtor(Stack* stk, ssize_t initCap) {
  ASSERT(stk != nullptr);

  StackError error = 0;

#ifdef _CANARY_PROT
  stk->lcanary = Canary;
  stk->rcanary = Canary;
#endif // _CANARY_PROT

  stk->size = 0;
  stk->capacity = (initCap < StandardAllocSize) ? StandardAllocSize : initCap;
  stk->data = (Elem*)calloc(sizeof(Elem)*(size_t)stk->capacity ON_CANARY(+ 2*sizeof(canary_t)), sizeof(Elem));

  FillBytes(AdrDataElem(stk, 0), &Nullifier, (size_t)stk->capacity, sizeof(Elem));

  if (stk->data == nullptr) {
    return error | StackState::ErrorCtor;
  }

#ifdef _CANARY_PROT
  memcpy(AdrLCanary(stk), &Canary, sizeof(canary_t)); // bytes misalligment fix
  memcpy(AdrRCanary(stk), &Canary, sizeof(canary_t));
#endif // _CANARY_PROT

#ifdef _HASH_PROT
  StackHashUpdate(stk);
#endif // _HASH_PROT

  return error;
}

StackError StackDtor(Stack* stk) {
  ASSERT(stk != nullptr);

  StackError error = 0;

  free(stk->data);
  stk->data = nullptr;

#ifdef _HASH_PROT
  stk->stackHash = 0;
  stk->dataHash = 0;
#endif

  stk->size = 0;
  stk->capacity = 0;

  return error;
}

StackError StackOk(Stack* stk) {
  StackError error = 0;

  if (stk == nullptr) {
    return error | StackState::ErrorStkNull;
  }
#ifdef _CANARY_PROT
  if (stk->lcanary != Canary) {
    return error |= StackState::ErrorStkLCanary;
  } else if (stk->rcanary != Canary) {
    return error |= StackState::ErrorStkRCanary;
  }
#endif // _CANARY_PROT

  if (stk->size < 0) {
    return error | StackState::ErrorStkSizeNeg;
  }

  if (stk->capacity < 0) {
    return error | StackState::ErrorStkCapNeg;
  }

#ifdef _HASH_PROT
  uint32_t holdHash = stk->stackHash;
  stk->stackHash = 0;

  uint32_t hashOk = Hash((const uint8_t*)stk, sizeof(Stack), 0);
  stk->stackHash = holdHash;
  if (stk->stackHash != hashOk) {
    return error |= StackState::ErrorStkHash;
  }
#endif // _HASH_PROT

  if (stk->data == nullptr) {
    return error | StackState::ErrorDataNull;
  }
  error |= StackOkData(stk);
  if (error != StackState::Success) {
      return error;
  }

  return error;
}

StackError StackPush(Stack* stk, Elem* value) {
  STACK_ASSERT(stk);

  StackError error = 0;

  error |= StackResizeUp(stk);

  memcpy(AdrDataElem(stk, stk->size), value, sizeof(Elem));
  (stk->size)++;

#ifdef _HASH_PROT
  error |= StackHashUpdate(stk);
#endif // _HASH_PROT

  STACK_ASSERT(stk);
  return error;
}

StackError StackPop(Stack* stk, Elem* retValue) {
  STACK_ASSERT(stk);

  StackError error = 0;

  if (stk->size <= 0) {
    error |= StackState::ErrorPopNoEl;
    return error;
  }

  --(stk->size);
  // *retValue = *AdrDataElem(stk, (stk->size));
  memcpy(retValue, AdrDataElem(stk, (stk->size)), sizeof(Elem));
  // *AdrDataElem(stk, stk->size) = (Elem)Nullifier;

  error |= StackResizeDown(stk);

#ifdef _HASH_PROT
  error |= StackHashUpdate(stk);
#endif // _HASH_PROT

  STACK_ASSERT(stk);
  return error;
}

void StackDump(Stack* stk, StackError error, const char* file, size_t line, const char* func) {
  ASSERT(stk != nullptr);

  fprintf(stderr, "\n");
  fprintf(stderr, "#  Stack[ %p ] called from file %s(%lu) from function %s\n", stk, file, line, func);
  fprintf(stderr, "#  Error: %lu\n", error);
  fprintf(stderr, "#  {\n");

#ifdef _CANARY_PROT
  fprintf(stderr, "#    lcanary 0x%lX\n", stk->lcanary);
#endif // _CANARY_PROT
  fprintf(stderr, "#    size %ld\n", stk->size);
  fprintf(stderr, "#    capacity %ld\n", stk->capacity);
#ifdef _HASH_PROT
  fprintf(stderr, "#    stackHash %u\n", stk->stackHash);
  fprintf(stderr, "#    dataHash %u\n", stk->dataHash);
#endif // _HASH_PROT

  fprintf(stderr, "#    data[ %p ]\n", stk->data);
  fprintf(stderr, "#    {\n");

#ifdef _CANARY_PROT
    fprintf(stderr, "#      lcanary 0x%lX\n", *(canary_t*)stk->data);
#endif // _CANARY_PROT
  for (ssize_t i = 0; i < stk->size; i++) {
    // fprintf(stderr, "#     *[%ld] %lf\n", i, *AdrDataElem(stk, i));
  }

  for (ssize_t i = stk->size; i < stk->capacity; i++) {
    // fprintf(stderr, "#      [%ld] 0x%X\n", i, *(unsigned int*)AdrDataElem(stk, i));
  }
#ifdef _CANARY_PROT
    fprintf(stderr, "#      rcanary 0x%lX\n", *AdrRCanary(stk));
#endif // _CANARY_PROT

  fprintf(stderr, "#    }\n");

#ifdef _CANARY_PROT
  fprintf(stderr, "#    rcanary 0x%lX\n", stk->rcanary);
#endif // _CANARY_PROT

  fprintf(stderr, "#  }\n");

  fprintf(stderr, "\n");
}

//---------------------------------------------------------------

static StackError StackResizeUp(Stack* stk) {
  ASSERT(stk != nullptr);

  StackError error = 0;

  if (stk->capacity <= stk->size) {
    stk->capacity *= MultiplyConst;
    error |= StackRecalloc(stk);
  }

  return error;
}

static StackError StackResizeDown(Stack* stk) {
  ASSERT(stk != nullptr);

  StackError error = 0;

  if (stk->size * MultiplyConst * MultiplyConst <= stk->capacity) {
    stk->capacity = stk->capacity > 1 ? stk->capacity / MultiplyConst : 1;
    error |= StackRecalloc(stk);
  }

  return error;
}

static StackError StackRecalloc(Stack* stk) {
  ASSERT(stk != nullptr);

  StackError error = 0;

  Elem* holdPtr = stk->data;
  stk->data = (Elem*)realloc(stk->data, (size_t)stk->capacity*sizeof(Elem) ON_CANARY(+ 2*sizeof(canary_t)));

  if (stk->data == nullptr) {
    stk->data = holdPtr;

    holdPtr = nullptr;
    error |= StackState::ErrorRecalloc;
    return error;
  }

  FillBytes(AdrDataElem(stk, stk->size), &Nullifier, (size_t)(stk->capacity - stk->size), sizeof(Elem));
#ifdef _CANARY_PROT
  memcpy(AdrRCanary(stk), &Canary, sizeof(canary_t));
#endif // _CANARY_PROT

  holdPtr = nullptr;

  return error;
}

static StackError StackOkData(Stack* stk) {
  ASSERT(stk != nullptr);

  StackError error = 0;

#ifdef _CANARY_PROT
  if (*AdrLCanary(stk) != Canary) {
    error |= StackState::ErrorDataLCanary;
    return error;
  // } else if (*AdrRCanary(stk) != Canary) {
  } else if (memcmp(AdrRCanary(stk), &Canary, sizeof(canary_t))) {
    error |= StackState::ErrorDataRCanary;
    return error;
  }
#endif

#ifdef _HASH_PROT
  uint32_t hashOkdata = 0;

  hashOkdata = Hash((const uint8_t*)stk->data, (size_t)stk->capacity*sizeof(Elem) ON_CANARY(+ 2*sizeof(canary_t)), 0);

  if (hashOkdata != stk->dataHash) {
    error |= StackState::ErrorDataHash;
    return error;
  }
#endif // _HASH_PROT

  return error;
}

#ifdef _HASH_PROT
static StackError StackHashUpdate(Stack* stk) {
  ASSERT(stk != nullptr);

  StackError error = 0;

  uint32_t holdHash = 0;

  stk->dataHash = 0;
  holdHash = Hash((const uint8_t*)stk->data, sizeof(Elem)*(size_t)stk->capacity ON_CANARY(+ 2*sizeof(canary_t)), 0);
  stk->dataHash = holdHash;

  stk->stackHash = 0;
  holdHash = Hash((const uint8_t*)stk, sizeof(Stack), 0);
  stk->stackHash = holdHash;
  holdHash = 0;

  return error;
}
#endif // _HASH_PROT

#ifdef _CANARY_PROT
static canary_t* AdrLCanary(Stack* stk) {
  ASSERT(stk != nullptr);

  return (canary_t*)stk->data;
}

static canary_t* AdrRCanary(Stack* stk) {
  ASSERT(stk != nullptr);

  return (canary_t*)((char*)stk->data + (size_t)stk->capacity*sizeof(Elem) + sizeof(canary_t));
}
#endif // _CANARY_PROT

static Elem* AdrDataElem(Stack* stk, ssize_t index) {
  ASSERT(stk != nullptr);

  return (Elem*)((char*)stk->data + (size_t)index*sizeof(Elem) ON_CANARY(+ sizeof(canary_t)));
}
