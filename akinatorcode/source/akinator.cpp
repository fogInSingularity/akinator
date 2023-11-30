#include "../include/akinator.h"

//static-----------------------------------------------------------------------

static size_t DropBuffer(FILE* file);
static void SayToUser(const wchar_t* msg);
static void ErrorMessage(const wchar_t* msg);
static bool GetLine(String* str);

//global-----------------------------------------------------------------------

Mode GetMode() {
  SayToUser(L"что вы хотите: [о]тгадать, [д]ать определение,\n"
            L"[с]равнить объекты, [п]оказать дерево,\n"
            L"[в]ыйти с сохранением или [б]ез него:\n");

  while (true) {
    wint_t ch = getwc(stdin);
    size_t droped = DropBuffer(stdin);

    if (droped != 0) {
      ch = L'\0';
    }

    switch (ch) {
      case L'о':
        return Mode::kGuess;
      case L'д':
        return Mode::kDefine;
      case L'с':
        return Mode::kCompare;
      case L'п':
        return Mode::kShowTree;
      case L'в':
        return Mode::kQuitAndSave;
      case L'б':
      case WEOF:
        return Mode::kQuitWithoutSave;
      default:
        SayToUser(L"не понимаю, введите ещё раз:\n");
        break;
    }
  }
}

//public-----------------------------------------------------------------------

AkinatorError Akinator::Start(int argc, const char** argv) {
  ASSERT(argv != nullptr);

  if (argc < 2) { return AkinatorError::kNotEnoughFiles; }
  db_file_name_ = argv[1];

  data_base_.Ctor();

  StringError str_error = StringError::kSuccess;
  str_error = raw_data_base_.Ctor();
  if (str_error != StringError::kSuccess) {
    return AkinatorError::kCantAllocDB;
  }

  FILE* db_file = fopen(db_file_name_, "rb");
  if (db_file == nullptr) { return AkinatorError::kCantOpenFile; }

  AkinatorError error = AkinatorError::kSuccess;
  error = LoadDB();
  if (error != AkinatorError::kSuccess) { return error; }

  data_base_.Traversal(&SetTypesOfEl); //NOTE

  // data_base_.DotDump(); // NOTE

  return AkinatorError::kSuccess;
}

void Akinator::End() {
  db_file_name_ = nullptr;

  raw_data_base_.Dtor();
  data_base_.DotDump();
  data_base_.Dtor(&ObjDtor);
}

void Akinator::ThrowError(AkinatorError error) {
  switch (error) {
    case AkinatorError::kSuccess:
      //---//
      break;
    case AkinatorError::kNotEnoughFiles:
      ErrorMessage(L"not enough files given");
      break;
    case AkinatorError::kCantOpenFile:
      ErrorMessage(L"cant open file");
      break;
    case AkinatorError::kCantAllocDB:
      ErrorMessage(L"cant allocate data base");
      break;
    case AkinatorError::kInvalidDataBase:
      ErrorMessage(L"invalid data base");
      break;
    case AkinatorError::kStackError:
      ErrorMessage(L"stack error");
      break;
    case AkinatorError::kStringError:
      ErrorMessage(L"string error");
      break;
    case AkinatorError::kDBStoreFailure:
      ErrorMessage(L"cant store data base");
      break;
    case AkinatorError::kBadTreeInsertion:
      ErrorMessage(L"bad insertion");
      break;
    default:
      ASSERT(0 && "UNKNOWN ERROR CODE");
      break;
  }
}

AkinatorError Akinator::ExecuteMode(Mode mode) {
  AkinatorError error = AkinatorError::kSuccess;

  switch (mode) {
    case Mode::kGuess:
      error = GuessMode();
      break;
    case Mode::kDefine:
      error = DefineMode();
      break;
    case Mode::kCompare:
      error = CompareMode();
      break;
    case Mode::kShowTree:
      error = ShowTreeMode();
      break;
    case Mode::kQuitAndSave:
      error = QuitAndSaveMode();
      break;
    case Mode::kQuitWithoutSave:
      error = QuitWithoutMode();
      break;
    default:
      ASSERT(0 && "UNKNOWN MODE");
      break;
  }

  return error;
}

//private----------------------------------------------------------------------

AkinatorError Akinator::LoadDB() {
  FILE* db_file = fopen(db_file_name_, "r");
  if (db_file == nullptr) { return AkinatorError::kCantOpenFile; }

  AkinatorError error = AkinatorError::kSuccess;
  error = LoadStrFromFile(db_file);
  if (error != AkinatorError::kSuccess) { return error; }

  TreeError tree_error = TreeError::kSuccess;
  tree_error = data_base_.LoadFromStr(&raw_data_base_);
  if (tree_error != TreeError::kSuccess) {
    return AkinatorError::kStringError;
  }

  fclose(db_file);
  return AkinatorError::kSuccess;
}

AkinatorError Akinator::StoreDB() {
  FILE* db_file = fopen(db_file_name_, "w");
  if (db_file == nullptr) { return AkinatorError::kCantOpenFile; }

  TreeError tree_error = TreeError::kSuccess;
  tree_error = data_base_.LoadToStr(&raw_data_base_);
  if (tree_error != TreeError::kSuccess) {
    return AkinatorError::kStringError;
  }

  AkinatorError error = AkinatorError::kSuccess;
  error = StoreStrToFile(db_file);
  if (error != AkinatorError::kSuccess) { return error; }

  fclose(db_file);
  return AkinatorError::kSuccess;
}

AkinatorError Akinator::LoadStrFromFile(FILE* file) {
  ASSERT(file != nullptr);

  wint_t ch = L'\0';
  StringError str_error = StringError::kSuccess;
  while ((ch = getwc(file)) != WEOF) {
    str_error = raw_data_base_.PushBack((wchar_t)ch);

    if (str_error != StringError::kSuccess) {
      return AkinatorError::kCantAllocDB;
    }
  }

  return AkinatorError::kSuccess;
}

AkinatorError Akinator::StoreStrToFile(FILE* file) {
  ASSERT(file != nullptr);

  wint_t ch = L'\0';
  wchar_t* ch_ptr = nullptr;
  for (Index i = 0; i < raw_data_base_.Size(); i++) {
    ch_ptr = raw_data_base_.At(i);
    if (ch_ptr == nullptr) { return AkinatorError::kDBStoreFailure; }

    ch = fputwc(*ch_ptr, file);
    if (ch == WEOF) { return AkinatorError::kDBStoreFailure; }
  }

  return AkinatorError::kSuccess;
}

AkinatorError Akinator::GuessMode() { //FIXME

  // StringError str_error = StringError::kSuccess;
  // str_error = item.str.Ctor(); //NOTE
  // if (str_error != StringError::kSuccess) { return AkinatorError::kStringError; }
  // item.type = TypeOfElem::kObject;

  Elem item = {};
  TreeError tree_error = TreeError::kSuccess;
  tree_error = data_base_.Insert(&item, &GuessIns);
  if (tree_error != TreeError::kSuccess) { return AkinatorError::kBadTreeInsertion; }

  return AkinatorError::kSuccess;
}

AkinatorError Akinator::DefineMode() {
  StringError str_error = StringError::kSuccess;

  Elem item = {};
  str_error = item.str.Ctor();
  if (str_error != StringError::kSuccess) { return AkinatorError::kStringError; }
  item.type = TypeOfElem::kObject;

  SayToUser(L"определение кого вы хотите узнать?:\n");
  bool get_error = GetLine(&item.str);
  if (get_error) {
    item.str.Dtor();
    return AkinatorError::kStringError;
  }

  const TreeNode* node = data_base_.Find(&item, &DefFind);
  if (node == nullptr) {
    item.str.Dtor();
    SayToUser(L"такого объекта нет\n");
    return AkinatorError::kSuccess;
  }

  Stack def_stk = {};
  StackError stk_error = 0;

  stk_error = StackCtor(&def_stk);
  if (stk_error != 0) { return AkinatorError::kStackError; }

  const TreeNode* iter_node = node;

  while (!data_base_.IsRoot(iter_node)) {
    stk_error = StackPush(&def_stk, &iter_node->parent->data);
    if (stk_error != 0) { return AkinatorError::kStackError; }

    iter_node = iter_node->parent;
  }

  SayToUser(item.str.Data());
  SayToUser(L" имеет свойсва:\n");
  Elem prop = {};
  while (def_stk.size != 0) {
    stk_error = StackPop(&def_stk, &prop);
    if (stk_error != 0) { return AkinatorError::kStackError; }
    SayToUser(prop.str.Data());
    SayToUser(L"\n");
  }

  item.str.Dtor();
  StackDtor(&def_stk);

  return AkinatorError::kSuccess;
}

AkinatorError Akinator::CompareMode() { //FIXME
  return AkinatorError::kSuccess;
}

AkinatorError Akinator::ShowTreeMode() { //FIXME
  return AkinatorError::kSuccess;
}

AkinatorError Akinator::QuitAndSaveMode() { //FIXME
  return AkinatorError::kSuccess;
}

AkinatorError Akinator::QuitWithoutMode() { //FIXME
  return AkinatorError::kSuccess;
}

//static-----------------------------------------------------------------------
static bool GetLine(String* str) {
  ASSERT(str != nullptr);

  wint_t ch = L'\0';
  StringError str_error = StringError::kSuccess;

  while ((ch = getwc(stdin)) != L'\n' && ch != WEOF) {
    str_error = str->PushBack((wchar_t)ch);
    if (str_error != StringError::kSuccess) { return true; }
  }

  return false;
}

static size_t DropBuffer(FILE* file) {
  ASSERT(file != nullptr);

  wint_t ch = L'\0';
  size_t dropped = 0;

  while ((ch = getwc(file)) != L'\n' && ch != WEOF) { dropped++; }

  return dropped;
}

static void SayToUser(const wchar_t* msg) {
  ASSERT(msg != nullptr);

  fputws(msg, stdout);
}

static void ErrorMessage(const wchar_t* msg) {
  ASSERT(msg != nullptr);

  fwprintf(stderr, RED BOLD L"error: " RESET L"%ls\n", msg);
}

//btree functions--------------------------------------------------------------

void ObjDtor(Elem* data) {
  ASSERT(data != nullptr);

  data->str.Dtor();
  data->type = TypeOfElem::kUninitType;
}

int DefFind(const void* a, const void* b) {
  ASSERT(a != nullptr);
  ASSERT(b != nullptr);

  const Elem* node_data = (const Elem*)a;
  const Elem* given_data = (const Elem*)b;

  if (node_data->type != TypeOfElem::kObject) { return 1; }
  return wcscmp(node_data->str.Data(), given_data->str.Data());
}

InsertRes GuessIns(TreeNode* node, Elem* elem) {
  ASSERT(node != nullptr);
  USE_VAR(elem);

  SayToUser(L"этот объект ");
  SayToUser(node->data.str.Data());
  SayToUser(L"?\n");

  StringError str_error = StringError::kSuccess;
  String ans = {};
  str_error = ans.Ctor();
  if (str_error != StringError::kSuccess) { return InsertRes::kError; }

  int ans_yes = 0;
  int ans_no = 0;
  while (true) {
    bool have_error = GetLine(&ans);
    if (have_error) {
      ans.Dtor();
      return InsertRes::kError;
    }

    ans_yes = wcscmp(ans.Data(), L"да");
    ans_no = wcscmp(ans.Data(), L"нет");

    if ((ans_yes != 0) && (ans_no != 0)) {
      SayToUser(L"я не понимаю\n");
      str_error = ans.Clear();
      if (str_error != StringError::kSuccess) {
        ans.Dtor();
        return InsertRes::kError;
      }
    } else {
      break;
    }
  }

  ans.Dtor(); //NOTE
  bool is_yes = !ans_yes;
  switch (node->data.type) {
    case TypeOfElem::kObject: {
      return GuessObj(node, is_yes);
      break;
    }
    case TypeOfElem::kProperty: {
      return GuessProp(node, is_yes);
      break;
    }
    case TypeOfElem::kUninitType: {
      return InsertRes::kError;
      break;
    }
    default:
      ASSERT(0 && "UNKNOWN ELEM TYPE");
      break;
  }

  return InsertRes::kError;
}

InsertRes GuessObj(TreeNode* node, bool is_yes) {
  if (is_yes) {
    SayToUser(L"я победил!!!\n");
    return InsertRes::kStopInsertion;
  }

  SayToUser(L"я проиграл(((\n");
  SayToUser(L"кто же это?\n");

  StringError str_error = StringError::kSuccess;
  bool have_error = false;

  Elem new_obj = {};
  str_error = new_obj.str.Ctor();
  if (str_error != StringError::kSuccess) { return InsertRes::kError; }
  new_obj.type = TypeOfElem::kObject;

  have_error = GetLine(&new_obj.str);
  if (have_error) {
    new_obj.str.Dtor();
    return InsertRes::kError;
  }

  SayToUser(L"чем ");
  SayToUser(new_obj.str.Data());
  SayToUser(L" отличается от ");
  SayToUser(node->data.str.Data());
  SayToUser(L"?\n");

  Elem new_prop = {};
  str_error = new_prop.str.Ctor();
  if (str_error != StringError::kSuccess) { return InsertRes::kError; }
  new_prop.type = TypeOfElem::kProperty;

  have_error = GetLine(&new_prop.str);
  if (have_error) {
    new_prop.str.Dtor();
    return InsertRes::kError;
  }

  TreeNode* prop_node = {};
  prop_node = GuessNodeCtor(nullptr, &new_prop);
  if (prop_node == nullptr) { return InsertRes::kError; }

  TreeNode* obj_node = {};
  obj_node = GuessNodeCtor(prop_node, &new_obj);
  if (obj_node == nullptr) {
    GuessNodeDtor(prop_node);//memory leak protection
    return InsertRes::kError;
  }

  TreeNode* parent_node = node->parent;
  if (parent_node != nullptr) {
    SetParentNode(parent_node, prop_node, node);
  }
  prop_node->l_child = node;
  prop_node->r_child = obj_node;
  node->parent = prop_node;
  obj_node->parent = prop_node;

  return InsertRes::kStopInsertion;
}

TreeNode* GuessNodeCtor(TreeNode* parent, Elem* elem) { //REVIEW
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

void GuessNodeDtor(TreeNode* node) {//REVIEW
  if (node == nullptr) { return; }

  node->data.str.Dtor();
  free(node);
  node = nullptr;
}

InsertRes GuessProp(TreeNode* node, bool is_yes) {
  ASSERT(node != nullptr);

  if (is_yes) {
    return GuessIns(node->r_child, nullptr);
  } else {
    return GuessIns(node->l_child, nullptr);
  }
}

void SetTypesOfEl(TreeNode* node) {
  // $
  if (node == nullptr) { return; }
  // $
  if ((node->l_child == nullptr) && (node->r_child == nullptr)) {
    node->data.type = TypeOfElem::kObject;
  } else {
    node->data.type = TypeOfElem::kProperty;
  }
}

void SetParentNode(TreeNode* parent_node,
                   TreeNode* new_child_node,
                   TreeNode* old_child_node) {
  ASSERT(parent_node != nullptr);
  ASSERT(new_child_node != nullptr);
  ASSERT(old_child_node != nullptr);

  new_child_node->parent = parent_node;
  if (parent_node->r_child == old_child_node) {
    parent_node->r_child = new_child_node;
  } else if (parent_node->l_child == old_child_node) {
    parent_node->l_child = new_child_node;
  } else {
    ASSERT(0 && "WTF");
  }
}
