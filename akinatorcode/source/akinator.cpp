#include "../include/akinator.h"
#include <cstdio>
#include <cwchar>

//static-----------------------------------------------------------------------

static size_t DropBuffer(FILE* file);
static void SayToUser(const wchar_t* msg);
static void ErrorMessage(const wchar_t* msg);
static bool GetLine(String* str);
static int Compare (const void* a, const void* b);

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

  data_base_.Ctor(Compare);

  StringError str_error = StringError::kSuccess;
  str_error = raw_data_base_.Ctor();
  if (str_error != StringError::kSuccess) {
    return AkinatorError::kCantAllocDB;
  }

  FILE* db_file = fopen(db_file_name_, "rb");
  if (db_file == nullptr) { return AkinatorError::kCantOpenFile; }

  AkinatorError error = AkinatorError::kSuccess;

  error = LoadDB();

  data_base_.DotDump();

  if (error != AkinatorError::kSuccess) { return error; }

  return AkinatorError::kSuccess;
}

void Akinator::End() {
  db_file_name_ = nullptr;

  db_file_name_ = nullptr;
  raw_data_base_.Dtor();
  data_base_.Dtor();
}

void Akinator::ThrowError(AkinatorError error) {
  // fwprintf(stderr, L"%d", error);
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

  wint_t ch = L'\0';
  StringError str_error = StringError::kSuccess;
  while ((ch = getwc(db_file)) != WEOF) {
    str_error = raw_data_base_.PushBack((wchar_t)ch);

    if (str_error != StringError::kSuccess) {
      fclose(db_file);
      return AkinatorError::kCantAllocDB;
    }
  }
  
  data_base_.LoadFromStr(&raw_data_base_);

  fclose(db_file);
  return AkinatorError::kSuccess;
}

AkinatorError Akinator::StoreDB() { //FIXME
  // FILE* db_file = fopen(db_file_name_, "w");
  // if (db_file == nullptr) { return AkinatorError::kCantOpenFile; }

  // wprintf(L"%ls", raw_data_base_.Data());

  return AkinatorError::kSuccess;
}

AkinatorError Akinator::GuessMode() {
  return AkinatorError::kSuccess;
}

AkinatorError Akinator::DefineMode() {
  Elem item = {};
  item.str.Ctor();
  item.type = TypeOfElem::kObject;

  bool get_error = GetLine(&item.str);
  if (!get_error) {
    item.str.Dtor();
    return AkinatorError::kStringError;
  }

  const TreeNode* node = data_base_.Find(&item);
  if (node == nullptr) {
    item.str.Dtor();
    SayToUser(L"такого объекта нет");
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

  //принтануть всю инфу со стека
  SayToUser(item.str.Data());
  SayToUser(L"имеет свосва:\n");
  Elem prop = {};
  while (def_stk.size != 0) {
    stk_error = StackPop(&def_stk, &prop);
    if (stk_error != 0) { return AkinatorError::kStackError; }
    SayToUser(prop.str.Data());
    SayToUser(L"\n");
  }

  item.str.Dtor();

  return AkinatorError::kSuccess;
}

AkinatorError Akinator::CompareMode() {
  return AkinatorError::kSuccess;
}

AkinatorError Akinator::ShowTreeMode() {
  return AkinatorError::kSuccess;
}

AkinatorError Akinator::QuitAndSaveMode() {
  return AkinatorError::kSuccess;
}

AkinatorError Akinator::QuitWithoutMode() {
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

static int Compare (const void* a, const void* b) { //FIXME
  (void)a;
  (void)b;
  return 0;
}
