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

  data_base_.DotDump(); // NOTE

  return AkinatorError::kSuccess;
}

void Akinator::End() {
  db_file_name_ = nullptr;

  raw_data_base_.Dtor();
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
  if (!get_error) {
    item.str.Dtor();
    return AkinatorError::kStringError;
  }

  const TreeNode* node = data_base_.Find(&item, &ObjFind);
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
    if (str_error != StringError::kSuccess) { return false; }
  }

  return true;
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

int ObjFind(const void* a, const void* b) {
  const Elem* node_data = (const Elem*)a;
  const Elem* given_data = (const Elem*)b;

  if (node_data->type != TypeOfElem::kObject) { return 1; }
  return wcscmp(node_data->str.Data(), given_data->str.Data());
}
