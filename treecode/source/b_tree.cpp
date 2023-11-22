#include "../include/b_tree.h"

//static-----------------------------------------------------------------------

static TreeNode* CtorNode(TreeNode* parent, Elem* elem);
static void DtorNode(TreeNode* node);

//public-----------------------------------------------------------------------

void BTree::Ctor() {
  root_ = nullptr;
}

void BTree::Dtor() {
  if (root_ != nullptr) {
    DtorNode(root_);
  }
}

TreeError BTree::Insert(Elem* elem) { // FIXME
  if (root_ == nullptr) {
    root_ = CtorNode(root_, elem);
    if (root_ == nullptr) { return TreeError::kNodeCtorBadAlloc; }
  } else {
    return InsertNode(root_, elem);
  }

  return TreeError::kSuccess;
}

TreeError BTree::Traversal(ActionFunc* Action) {
  return NodeTraversal(root_, Action);
}

TreeError BTree::LoadToFile(FILE* file) {
  LoadNodeTo(file, root_);

  return TreeError::kSuccess;
}

TreeError BTree::LoadFromFile(FILE* file) {
  // BinData data = {};
  // data.GetData(file);

  // //process data

  // data.FreeData();

  return TreeError::kSuccess;
}

//private----------------------------------------------------------------------

TreeError BTree::InsertNode(TreeNode* node, Elem* elem) {
  // if (CompareFunc(node.data, elem)) { // TODO:
  //   if (node->r_child != nullptr) {
  //     return InsertNode(node->r_child, elem);
  //   } else {
  //     node->r_child = CtorNode(node->r_child, elem);
  //     if (node->r_child == nullptr) { return TreeError::kNodeCtorBadAlloc; }
  //   }
  // } else {
  //   if (node->l_child != nullptr) {
  //     return InsertNode(node->l_child, elem);
  //   } else {
  //     node->l_child = CtorNode(node->l_child, elem);
  //     if (node->l_child == nullptr) { return TreeError::kNodeCtorBadAlloc; }
  //   }
  // }

  return TreeError::kSuccess;
}

TreeError BTree::NodeTraversal(TreeNode* node, ActionFunc* Action) {
  Action(node);
  if (node == nullptr) { return TreeError::kSuccess; }

  NodeTraversal(node->l_child, Action);
  NodeTraversal(node->r_child, Action);

  return TreeError::kSuccess;
}

TreeError BTree::LoadNodeTo(FILE* file, TreeNode* node) {
  if (node != nullptr) {
    fputc('(', file);
    fprintf(file, "%d", node->data);
    LoadNodeTo(file, node->l_child);
    LoadNodeTo(file, node->r_child);
    fputc(')', file);
  } else {
    fputc('*', file);
  }

  return TreeError::kSuccess;
}

//static-----------------------------------------------------------------------

static TreeNode* CtorNode(TreeNode* parent, Elem* elem) {
  TreeNode* new_node = (TreeNode*)calloc(1, sizeof(TreeNode));
  if (new_node == nullptr) { return nullptr; }

  new_node->data = *elem;
  new_node->l_child = nullptr;
  new_node->r_child = nullptr;
  new_node->parent = parent;

  return new_node;
}

static void DtorNode(TreeNode* node) {
  if (node == nullptr) {
    return;
  }

  DtorNode(node->l_child);
  DtorNode(node->r_child);

  free(node);
}
