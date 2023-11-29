#include "../include/b_tree.h"
#include <cstddef>
#include <endian.h>

//static-----------------------------------------------------------------------

const char* kDotName = "dotdump.dot";

static const wchar_t* SkipSpaces(const wchar_t* move);

//public-----------------------------------------------------------------------

void BTree::Ctor() {
  InsertCond_ = nullptr;
  FindCond_ = nullptr;
  Action_ = nullptr;

  root_ = nullptr;
}

void BTree::Dtor(ElemDtorFunc* ElemD) {
  ASSERT(ElemD != nullptr);

  ElemDtor_ = ElemD;
  DtorNode(root_);
  root_ = nullptr;
  ElemDtor_ = nullptr;

  Action_ = nullptr;
  FindCond_ = nullptr;
  InsertCond_ = nullptr;
}

TreeError BTree::DotDump() { //FIXME dump func
  FILE* dot_file = fopen(kDotName, "w");
  if (dot_file == nullptr) { return TreeError::kBadFileAccess; }

  fprintf(dot_file, "digraph {\n");
  fprintf(dot_file, "rankdir=LR\n");
  NodeDotDump(dot_file, root_);
  fprintf(dot_file, "}\n");

  fclose(dot_file);
  return TreeError::kSuccess;
}

TreeError BTree::Insert(Elem* elem, InsertCondFunc* Insrt) {
  ASSERT(elem != nullptr);
  ASSERT(Insrt != nullptr);

  InsertCond_ = Insrt;

  if (root_ == nullptr) {
    root_ = CtorNode(root_, elem);
    if (root_ == nullptr) { return TreeError::kNodeCtorBadAlloc; }
  } else {
    return InsertNode(root_, elem);
  }

  InsertCond_ = nullptr;

  return TreeError::kSuccess;
}

const TreeNode* BTree::Find(Elem* item, FindCondFunc* Fnd) {
  ASSERT(item != nullptr);
  ASSERT(Fnd != nullptr);

  FindCond_ = Fnd;

  const TreeNode* node = FindNode(root_, item);

  FindCond_ = nullptr;

  return node;
}

TreeError BTree::Traversal(ActionFunc* Act) {
  ASSERT(Act != nullptr);

  Action_ = Act;

  TreeError tree_error = TreeError::kSuccess;

  tree_error = NodeTraversal(root_);
  if (tree_error != TreeError::kSuccess) { return tree_error; }

  Action_ = nullptr;

  return TreeError::kSuccess;
}

bool BTree::IsRoot(const TreeNode* node) {
  ASSERT(node != nullptr);

  return (node == root_);
}

TreeError BTree::LoadToStr(String* str) {//cringe
  ASSERT(str != nullptr);

  return LoadNodeToStr(str, root_);
}

TreeError BTree::LoadFromStr(String* str) {//cringe?
  ASSERT(str != nullptr)

  if (!IsValid(str)) { return TreeError::kInvalidTree; }

  if (root_ != nullptr) { return TreeError::kInvalidTree; }

  Counter shift = 0;
  root_ = LoadNodeFromStr(str->Data(), &shift, nullptr);
  if (root_ == nullptr) { return TreeError::kInvalidTree; }

  return TreeError::kSuccess;
}

//private----------------------------------------------------------------------

TreeNode* BTree::CtorNode(TreeNode* parent, Elem* elem) {
  ASSERT(elem != nullptr);
  // parent node can be nullptr

  TreeNode* new_node = (TreeNode*)calloc(1, sizeof(TreeNode));
  if (new_node == nullptr) { return nullptr; }

  new_node->data = *elem;
  new_node->l_child = nullptr;
  new_node->r_child = nullptr;
  new_node->parent = parent;

  return new_node;
}

void BTree::DtorNode(TreeNode* node) {
  ASSERT(ElemDtor_ != nullptr);

  if (node == nullptr) { return; }

  DtorNode(node->l_child);
  DtorNode(node->r_child);

  ElemDtor_(&node->data);
  free(node);
  node = nullptr;
}

void BTree::NodeDotDump(FILE* file, TreeNode* node) { //FIXME dump func
  ASSERT(file != nullptr);

  if (node == nullptr) { return; }

  if (node->l_child != nullptr) {
    fprintf(file, "node%lu->node%lu\n", (size_t)node, (size_t)node->l_child);
  }
  if (node->r_child != nullptr) {
    fprintf(file, "node%lu->node%lu\n", (size_t)node, (size_t)node->r_child);
  }

  NodeDotDump(file, node->l_child);
  NodeDotDump(file, node->r_child);
}

TreeError BTree::InsertNode(TreeNode* node, Elem* elem) {
  ASSERT(node != nullptr);
  ASSERT(elem != nullptr);
  ASSERT(InsertCond_ != nullptr);

  int ins_res = InsertCond_(node, elem);

  if (ins_res > 0) {
    if (node->r_child != nullptr) {
      return InsertNode(node->r_child, elem);
    }

    node->r_child = CtorNode(node->r_child, elem);
    if (node->r_child == nullptr) { return TreeError::kNodeCtorBadAlloc; }
  } else if (ins_res < 0) {
    if (node->l_child != nullptr) {
      return InsertNode(node->l_child, elem);
    }

    node->l_child = CtorNode(node->l_child, elem);
    if (node->l_child == nullptr) { return TreeError::kNodeCtorBadAlloc; }
  } else {
    return TreeError::kBadInsertion;
  }

  return TreeError::kSuccess;
}

const TreeNode* BTree::FindNode(TreeNode* node, Elem* item) {
  ASSERT(item != nullptr);
  ASSERT(FindCond_ != nullptr);

  if (node == nullptr) { return nullptr; }

  if (FindCond_(&node->data, item) == 0) { return node; }

  const TreeNode* left_search = FindNode(node->l_child, item);
  if ( left_search != nullptr) { return left_search; }
  const TreeNode* right_search = FindNode(node->r_child, item);
  if ( right_search != nullptr) { return right_search; }

  return nullptr;
}

TreeError BTree::NodeTraversal(TreeNode* node) {
  ASSERT(Action_ != nullptr);

  Action_(node);
  if (node == nullptr) { return TreeError::kSuccess; }

  NodeTraversal(node->l_child);
  NodeTraversal(node->r_child);

  return TreeError::kSuccess;
}

TreeError BTree::LoadNodeToStr(String* str, TreeNode* node) {//cringe
  ASSERT(str != nullptr);
  ASSERT(node != nullptr);

  StringError str_error = StringError::kSuccess;
  TreeError tree_error = TreeError::kSuccess;

  if (node != nullptr) {
    str_error = PushNodeToStr(str, node);
    if (str_error != StringError::kSuccess) { return TreeError::kStringError; }

    tree_error = LoadNodeToStr(str, node->l_child);
    if (tree_error != TreeError::kSuccess) { return tree_error; }
    tree_error = LoadNodeToStr(str, node->r_child);
    if (tree_error != TreeError::kSuccess) { return tree_error; }

    str_error = str->PushBack(L')');
    if (str_error != StringError::kSuccess) { return TreeError::kStringError; }
  } else {
    str_error = str->PushBack(L'_');
    if (str_error != StringError::kSuccess) { return TreeError::kStringError; }
  }

  return TreeError::kSuccess;
}

StringError BTree::PushNodeToStr(String* str, TreeNode* node) {//cringe
  ASSERT(str != nullptr);
  ASSERT(node != nullptr);

  StringError str_error = StringError::kSuccess;

  str_error = str->Reserve(node->data.str.Length() + 4);
  if (str_error != StringError::kSuccess) { return str_error; }

  str_error = str->PushBack(L'(');
  if (str_error != StringError::kSuccess) { return str_error; }

  str_error = str->PushBack(L'\"');
  if (str_error != StringError::kSuccess) { return str_error; }

  str_error = str->Append(node->data.str.Data());
  if (str_error != StringError::kSuccess) { return str_error; }

  str_error = str->PushBack(L'\"');
  if (str_error != StringError::kSuccess) { return str_error; }

  return StringError::kSuccess;
}

TreeNode* BTree::LoadNodeFromStr(const wchar_t* str, Counter* shift, TreeNode* parent) {//cringe
  ASSERT(str != nullptr);
  ASSERT(shift != nullptr);
  // parent can be nullptr

  const wchar_t* left_quat = wcschr(str + *shift, L'\"');
  const wchar_t* right_quat = wcschr(left_quat + 1, L'\"');
  *shift += right_quat - (str + *shift) + 1;

  Elem data = {};
  data.str.Ctor((size_t)(right_quat - left_quat - 1), left_quat + 1);

  TreeNode* new_node = {};
  if (*(str + *shift) == L'_') {
    data.type = TypeOfElem::kObject;
    new_node = CtorNode(parent, &data);
    *shift += 3;
    return new_node;
  } else {
    data.type = TypeOfElem::kProperty;
    new_node = CtorNode(parent, &data);
    if (new_node == nullptr) { return nullptr; }

    new_node->l_child = LoadNodeFromStr(str, shift, new_node);
    if (new_node->l_child == nullptr) {
      DtorNode(new_node);
      return nullptr;
    }

    new_node->r_child = LoadNodeFromStr(str, shift, new_node);
    if (new_node->r_child == nullptr) {
      DtorNode(new_node);
      return nullptr;
    }
  }

  return new_node;
}

bool BTree::IsValid(String* raw_tree) {//cringe mb valid func?
  ASSERT(raw_tree != nullptr);

  Counter brackets = 0;
  Counter quat     = 0;
  Counter star     = 0;

  Counter error    = 0;

  const wchar_t* str = raw_tree->Data();
  while (*str != L'\0') {
    brackets += (*str == L'(') - (*str == L')');
    quat     += (*str == L'\"');
    star     += (*str == L'*');

    error    += (*str == L')') * (quat % 2);

    str++;
  }

  return !brackets && (quat % 2 == 0) && (star % 2 == 0) && !error;
}

//static-----------------------------------------------------------------------
__attribute__((__unused__))
static const wchar_t* SkipSpaces(const wchar_t* move) {
  ASSERT(move != nullptr);

  while (iswspace((wint_t)*move)) { move++; }

  return move;
}
