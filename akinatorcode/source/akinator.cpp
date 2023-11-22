#include "../include/akinator.h"

//static-----------------------------------------------------------------------

static size_t DropBuffer(FILE* file);

static void RequestMode();
static void IncorrectInput();
static void ErrorMessage(const wchar_t* msg);

//global-----------------------------------------------------------------------

Mode GetMode() {
  RequestMode();

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
        IncorrectInput();
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

  

  return AkinatorError::kSuccess;
}

void Akinator::End() {
  db_file_name_ = nullptr;

  raw_data_base_.Dtor();
  data_base_.Dtor();
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

static size_t DropBuffer(FILE* file) {
  ASSERT(file != nullptr);

  wint_t ch = L'\0';
  size_t dropped = 0;

  while ((ch = getwc(file)) != L'\n' && ch != WEOF) { dropped++; }

  return dropped;
}

static void RequestMode() {
  wprintf(L"что вы хотите: [о]тгадать, [д]ать определение,\n"
          L"[с]равнить объекты, [п]оказать дерево,\n"
          L"[в]ыйти с сохранением или [б]ез него:\n");
}

static void IncorrectInput() {
  wprintf(L"не понимаю, введите ещё раз:\n");
}

static void ErrorMessage(const wchar_t* msg) {
  fwprintf(stderr, RED BOLD "error: " RESET "%s\n", msg);
}
