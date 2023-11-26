#ifndef LIB_BTREE_H_
#define LIB_BTREE_H_

#include <stddef.h>
#include <stdlib.h>
#include <wctype.h>

#include "tree_config.h"
#include "../../libcode/include/lib_config.h"

#include "../../libcode/include/my_assert.h"
#include "../../libcode/include/my_typedefs.h"
#include "../../libcode/include/debug.h"

enum class TreeError {
  kSuccess          = 0,
  kCtorBadAlloc     = 1,
  kNodeCtorBadAlloc = 2,
  kStringError      = 3,
  kInvalidTree      = 4,
  kBadFileAccess    = 5,
};

struct TreeNode {
  Elem data;
  TreeNode* l_child;
  TreeNode* r_child;
  TreeNode* parent;
};

typedef void ActionFunc(TreeNode* node);

struct BTree {
 public: //--------------------------------------------------------------------
  void Ctor(CompareFunc* cmp);
  void Dtor();
  TreeError DotDump();

  TreeError Insert(Elem* elem);
  const TreeNode* Find(Elem* item);
  TreeError Traversal(ActionFunc* Action);
  bool IsRoot(const TreeNode* node);
  TreeError LoadToStr(String* str);
  TreeError LoadFromStr(String* str);
 private: //-------------------------------------------------------------------
  TreeNode* CtorNode(TreeNode* parent, Elem* elem);
  void DtorNode(TreeNode* node);
  void NodeDotDump(FILE* file, TreeNode* node);

  TreeError InsertNode(TreeNode* node, Elem* elem);
  const TreeNode* FindNode(TreeNode* node, Elem* item);
  TreeError NodeTraversal(TreeNode* node, ActionFunc* Action);
  TreeError LoadNodeToStr(String* str, TreeNode* node);
  TreeError LoadNodeFromStr(const wchar_t* str, TreeNode** node, TreeNode* parent);
  bool IsValid(String* raw_tree);

  CompareFunc* compare_;
  // ActionFunc action;
  TreeNode* root_;
};

#endif // LIB_BTREE_H_
