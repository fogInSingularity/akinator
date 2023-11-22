#ifndef LIB_BTREE_H_
#define LIB_BTREE_H_

#include <stddef.h>
#include <stdlib.h>

#include "tree_config.h"
#include "../../libcode/include/lib_config.h"

#include "../../libcode/include/my_assert.h"

enum class TreeError {
  kSuccess          = 0,
  kCtorBadAlloc     = 1,
  kNodeCtorBadAlloc = 2,
};



struct TreeNode {
  Elem data;
  TreeNode* l_child;
  TreeNode* r_child;
  TreeNode* parent;
};

typedef void ActionFunc(TreeNode* node);

struct BTree {
 public:
  void Ctor();
  void Dtor();

  TreeError Insert(Elem* elem);
  TreeError Traversal(ActionFunc* Action);
  TreeError LoadToFile(FILE* file);
  TreeError LoadFromFile(FILE* file);
 private:
  TreeError InsertNode(TreeNode* node, Elem* elem);
  TreeError NodeTraversal(TreeNode* node, ActionFunc* Action);
  TreeError LoadNodeTo(FILE* file, TreeNode* node);
  TreeNode* root_;
};

#endif // LIB_BTREE_H_
