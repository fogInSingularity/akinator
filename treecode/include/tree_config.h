#ifndef TREE_CONFIG_H_
#define TREE_CONFIG_H_

#include "../../libcode/include/my_string.h"

// typedef int Elem;

enum class TypeOfElem {
  kObject   = 0,
  kProperty = 1,
};

struct Elem {
  String str;
  TypeOfElem type;
};

#endif // TREE_CONFIG_H_
