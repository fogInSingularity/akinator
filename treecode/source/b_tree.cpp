#include "../include/b_tree.h"

//static-----------------------------------------------------------------------

const char* kDotName = "dotdump.dot";

static const wchar_t* SkipSpaces(const wchar_t* move);

//public-----------------------------------------------------------------------

void BTree::Ctor(CompareFunc* cmp) {
  ASSERT(cmp != nullptr);

  compare_ = cmp;
  root_ = nullptr;
}

void BTree::Dtor() {
  if (root_ != nullptr) {
    DtorNode(root_);
  }
}

TreeError BTree::DotDump() {
  FILE* dot_file = fopen(kDotName, "w");
  if (dot_file == nullptr) { return TreeError::kBadFileAccess; }

  fprintf(dot_file, "digraph {\n");
  fprintf(dot_file, "rankdir=LR\n");
  NodeDotDump(dot_file, root_);
  fprintf(dot_file, "}\n");

  fclose(dot_file);
  return TreeError::kSuccess;
}

TreeError BTree::Insert(Elem* elem) {
  ASSERT(elem != nullptr);

  if (root_ == nullptr) {
    root_ = CtorNode(root_, elem);
    if (root_ == nullptr) { return TreeError::kNodeCtorBadAlloc; }
  } else {
    return InsertNode(root_, elem);
  }

  return TreeError::kSuccess;
}

const TreeNode* BTree::Find(Elem* item) {
  ASSERT(item != nullptr);

  return FindNode(root_, item);
}

TreeError BTree::Traversal(ActionFunc* Action) {
  ASSERT(Action != nullptr);

  return NodeTraversal(root_, Action);
}

bool BTree::IsRoot(const TreeNode* node) {
  ASSERT(node != nullptr);

  return (node == root_);
}

TreeError BTree::LoadToStr(String* str) {
  ASSERT(str != nullptr);

  return LoadNodeToStr(str, root_);
}

TreeError BTree::LoadFromStr(String* str) {
  ASSERT(str != nullptr)

  if (!IsValid(str)) { return TreeError::kInvalidTree; }

  return LoadNodeFromStr(str->Data(), &root_, root_);
}

//private----------------------------------------------------------------------

TreeNode* BTree::CtorNode(TreeNode* parent, Elem* elem) {
  // ASSERT(parent != nullptr);
  ASSERT(elem != nullptr);

  TreeNode* new_node = (TreeNode*)calloc(1, sizeof(TreeNode));
  if (new_node == nullptr) { return nullptr; }

  new_node->data = *elem;
  new_node->l_child = nullptr;
  new_node->r_child = nullptr;
  new_node->parent = parent;

  return new_node;
}

void BTree::DtorNode(TreeNode* node) {
  if (node == nullptr) { return; }

  DtorNode(node->l_child);
  DtorNode(node->r_child);

  free(node);
}

void BTree::NodeDotDump(FILE* file, TreeNode* node) {
  if (node == nullptr) { return; }

  if (node->l_child != nullptr) {
    fprintf(file, "node%lu->node%lu\n", node, node->l_child);
  }
  if (node->r_child != nullptr) {
    fprintf(file, "node%lu->node%lu\n", node, node->r_child);
  }

  NodeDotDump(file, node->l_child);
  NodeDotDump(file, node->r_child);
  fprintf(file, "node%lu->node%lu\n", node->parent, node);
}

TreeError BTree::InsertNode(TreeNode* node, Elem* elem) {
  ASSERT(node != nullptr);
  ASSERT(elem != nullptr);

  if (compare_(node, elem) > 0) { //TODO
    if (node->r_child != nullptr) {
      return InsertNode(node->r_child, elem);
    }

    node->r_child = CtorNode(node->r_child, elem);
    if (node->r_child == nullptr) { return TreeError::kNodeCtorBadAlloc; }
  } else {
    if (node->l_child != nullptr) {
      return InsertNode(node->l_child, elem);
    }

    node->l_child = CtorNode(node->l_child, elem);
    if (node->l_child == nullptr) { return TreeError::kNodeCtorBadAlloc; }
  }

  return TreeError::kSuccess;
}

const TreeNode* BTree::FindNode(TreeNode* node, Elem* item) {
  ASSERT(node != nullptr);
  ASSERT(item != nullptr);

  if (item == nullptr) { return nullptr; }
  if (compare_(&node->data, item) == 0) { return node; }

  const TreeNode* left_search = FindNode(node->l_child, item);
  if ( left_search != nullptr) { return left_search; }
  const TreeNode* right_search = FindNode(node->r_child, item);
  if ( right_search != nullptr) { return right_search; }

  return nullptr;
}

TreeError BTree::NodeTraversal(TreeNode* node, ActionFunc* Action) {
  ASSERT(node != nullptr);
  ASSERT(Action != nullptr);

  Action(node);
  if (node == nullptr) { return TreeError::kSuccess; }

  NodeTraversal(node->l_child, Action);
  NodeTraversal(node->r_child, Action);

  return TreeError::kSuccess;
}

TreeError BTree::LoadNodeToStr(String* str, TreeNode* node) {
  ASSERT(str != nullptr);
  ASSERT(node != nullptr);

  StringError str_error = StringError::kSuccess;
  TreeError tree_error = TreeError::kSuccess;

  if (node != nullptr) {
    str_error = str->PushBack(L'(');
    if (str_error != StringError::kSuccess) { return TreeError::kStringError; }

    str->PushBack(L'\"');
    str->Append(node->data.str.Data());
    str->PushBack(L'\"');

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

  return  TreeError::kSuccess;
}

  // ASSERT(str != nullptr);
  // ASSERT(node != nullptr);

  // str = SkipSpaces(str);
  // if (*str != L'(') { return TreeError::kInvalidTree; }

  // str++;
  // if (*str != L'\"') { return TreeError::kInvalidTree; }

  // const wchar_t* left_quat = str;
  // const wchar_t* right_quat = wcschr(str + 1, L'\"');

  // Elem data = {};
  // StringError str_error = StringError::kSuccess;
  // str_error = data.str.Ctor(left_quat + 1, (size_t)(right_quat - left_quat - 1));
  // if (str_error != StringError::kSuccess) { return TreeError::kInvalidTree; }

  // str = right_quat + 1;
  // str = SkipSpaces(str);

  // //FIXME
  // const wchar_t* left_bracket = wcschr(str, L'(');
  // const wchar_t* right_bracket = wcschr(str, L')');
  // const wchar_t* first_star    = wcschr(str, L'*');
  // if (left_bracket > first_star) {
  //   data.type = TypeOfElem::kObject;
  //   TreeNode* new_node = CtorNode(*node, &data);
  //   if (new_node == nullptr) { return TreeError::kNodeCtorBadAlloc; }

  //   new_node->l_child = nullptr;
  //   new_node->r_child = nullptr;
  // } else {
  //   data.type = TypeOfElem::kProperty;
  //   TreeNode* new_node = CtorNode(*node, &data);
  //   TreeError tree_error = TreeError::kSuccess;
  //   tree_error = LoadNodeFromStr(left_bracket, &new_node->l_child);
  //   if (tree_error != TreeError::kSuccess) { return tree_error; }

  //   tree_error = LoadNodeFromStr(right_bracket + 1, &new_node->r_child);
  //   if (tree_error != TreeError::kSuccess) { return tree_error; }
  // }

TreeError BTree::LoadNodeFromStr(const wchar_t* str, TreeNode** node, TreeNode* parent) {
  if (*str == L'\0') { return TreeError::kSuccess; }

  Elem data = {};
  const wchar_t* left_quat = wcschr(str, L'\"');
  const wchar_t* right_quat = wcschr(left_quat + 1, L'\"');

  data.str.Ctor(right_quat - left_quat - 1, left_quat + 1);

  if (*(right_quat + 1) == L'*') {
    data.type = TypeOfElem::kObject;
    TreeNode* new_node = CtorNode(parent, &data);
  } else {
    data.type = TypeOfElem::kProperty;
    TreeNode* new_node = CtorNode(parent, &data);
    LoadFromStr(right_quat + 1, &new_node->l_child, new_node);
  }


  // LoadNodeFromStr(str + 1, &new_node->l_child, new_node);
  // LoadNodeFromStr(str + 1, &new_node->r_child, new_node);



  *node = new_node;

  return TreeError::kSuccess;
}

bool BTree::IsValid(String* raw_tree) {
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

static const wchar_t* SkipSpaces(const wchar_t* move) {
  ASSERT(move != nullptr);

  while (iswspace((wint_t)*move)) { move++; }

  return move;
}
