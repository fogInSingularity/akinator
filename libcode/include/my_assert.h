#ifndef LIB_MYASSERT_H_
#define LIB_MYASSERT_H_

#include <stdio.h>
#include <wchar.h>

#include "lib_config.h"

#include "color.h"

#ifdef ASSERT_ON
#define ASSERT(expr)                                                           \
  if ((expr) == 0) {                                                           \
    fwprintf(stderr, BOLD RED L"#  ERROR:\n");                                 \
    fwprintf(stderr,          L"#    %s\n", #expr);                            \
    fwprintf(stderr,          L"#    file: %s\n", __FILE__);                   \
    fwprintf(stderr,          L"#    func: %s\n" RESET, __PRETTY_FUNCTION__);  \
    abort();                                                                   \
  }
#else
#define ASSERT(expr) ;
#endif // ASSERT_ON

#endif // LIB_MYASSERT_H_
