#ifndef TREE_CONFIG_H_
#define TREE_CONFIG_H_

#include "../../libcode/include/my_string.h"

enum class TypeOfElem {
  kUninitType = 0,
  kObject     = 1,
  kProperty   = 2,
};

struct Elem {
  String str;
  TypeOfElem type;
};

#endif // TREE_CONFIG_H_
