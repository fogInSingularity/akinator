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
  kBadInsertion     = 6,
};

struct TreeNode {
  Elem data;
  TreeNode* l_child;
  TreeNode* r_child;
  TreeNode* parent;
};

enum class InsertRes {
  kError         = 0,
  kLeftNode      = 1,
  kRightNode     = 2,
  kStopInsertion = 3,
};

typedef void ActionFunc(TreeNode* node);
typedef void ElemDtorFunc(Elem* data);
typedef CompareFunc FindCondFunc;
typedef InsertRes InsertCondFunc(TreeNode* node, Elem* elem);

struct BTree {
 public: //--------------------------------------------------------------------
  void Ctor();
  void Dtor(ElemDtorFunc* ElemD);
  TreeError DotDump();

  TreeError Insert(Elem* elem, InsertCondFunc* Insrt);
  const TreeNode* Find(Elem* item, FindCondFunc* Fnd);
  TreeError Traversal(ActionFunc* Act);
  bool IsRoot(const TreeNode* node);

  TreeError LoadToStr(String* str);
  TreeError LoadFromStr(String* str);
 private: //-------------------------------------------------------------------
  TreeNode* CtorNode(TreeNode* parent, Elem* elem);
  void DtorNode(TreeNode* node);
  void NodeDotDump(FILE* file, TreeNode* node);

  TreeError InsertNode(TreeNode* node, Elem* elem);
  const TreeNode* FindNode(TreeNode* node, Elem* item);
  TreeError NodeTraversal(TreeNode* node);

  TreeError LoadNodeToStr(String* str, TreeNode* node);
  StringError PushNodeToStr(String* str, TreeNode* node);
  TreeNode* LoadNodeFromStr(const wchar_t* str, Counter* shift, TreeNode* parent);
  bool IsValid(String* raw_tree);

  // CompareFunc* compare_;
  InsertCondFunc* InsertCond_;
  FindCondFunc* FindCond_; // == 0 means that it is correct obj
  ActionFunc* Action_;
  ElemDtorFunc* ElemDtor_;

  TreeNode* root_;
};

#endif // LIB_BTREE_H_
