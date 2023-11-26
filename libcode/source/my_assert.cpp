#include "../include/my_assert.h"

void my_assert(bool expr, const char* expr_str, const char* file, const char* func) {
  if (expr) { return; }

  (void)fwprintf(stderr, BOLD RED L"#  ERROR:\n");
  (void)fwprintf(stderr,          L"#    %s\n", expr_str);
  (void)fwprintf(stderr,          L"#    file: %s\n", file);
  (void)fwprintf(stderr,          L"#    func: %s\n" RESET, func);
  abort();
}
