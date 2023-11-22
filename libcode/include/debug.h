#ifndef LIB_DEBUG_H_
#define LIB_DEBUG_H_

#include <stdio.h>
#include <wchar.h>

#include "lib_config.h"

#include "color.h"

#ifdef DEBUG_ON
#define PRINT_BYTE(n)   fwprintf(stderr, L">>> %x <<<\n", n);
#define PRINT_INT(n)    fwprintf(stderr, L">>> %d <<<\n", n);
#define PRINT_UINT(n)   fwprintf(stderr, L">>> %u <<<\n", n);
#define PRINT_LONG(n)   fwprintf(stderr, L">>> %ld <<<\n", n);
#define PRINT_ULONG(n)  fwprintf(stderr, L">>> %lu <<<\n", n);
#define PRINT_ULX(n)    fwprintf(stderr, L">>> %lX <<<\n", n);
#define PRINT_DOUBLE(n) fwprintf(stderr, L">>> %lf <<<\n", n);
#define PRINT_FLOAT(n)  fwprintf(stderr, L">>> %d <<<\n", n);

#define $         fwprintf(stderr, BOLD MAGENTA L">>> %s(%d) %s\n"  RESET,    \
                         __FILE__, __LINE__, __PRETTY_FUNCTION__);
#define $$(...) { fwprintf(stderr, BOLD MAGENTA L">>> %s\n" RESET,            \
                          #__VA_ARGS__);                                      \
                  __VA_ARGS__; }
#else
#define PRINT_BYTE(n)   ;
#define PRINT_INT(n)    ;
#define PRINT_UINT(n)   ;
#define PRINT_LONG(n)   ;
#define PRINT_ULONG(n)  ;
#define PRINT_ULX(n)    ;
#define PRINT_DOUBLE(n) ;
#define PRINT_FLOAT(n)  ;

#define $ ;
#define $$(...) { __VA_ARGS__; }
#endif // DEBUG_ON

#endif // LIB_DEBUG_H_
