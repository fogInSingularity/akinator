#ifndef AKINATOR_H_
#define AKINATOR_H_

#include <wchar.h>
#include <stddef.h>
#include <stdio.h>

#include "../../treecode/include/b_tree.h"
#include "../../libcode/include/debug.h"
#include "../../libcode/include/my_string.h"

enum class Mode {
  kGuess           = 0,
  kDefine          = 1,
  kCompare         = 2,
  kShowTree        = 3,
  kQuitAndSave     = 4,
  kQuitWithoutSave = 5,
};

enum class AkinatorError {
  kSuccess         = 0,
  kNotEnoughFiles  = 1,
  kCantOpenFile    = 2,
  kCantAllocDB     = 3,
  kInvalidDataBase = 4,
};

struct Akinator {
 public:
  AkinatorError Start(int argc, const char** argv);
  void End();
  void ThrowError(AkinatorError error);

  AkinatorError ExecuteMode(Mode mode);
 private:
  AkinatorError LoadDB();
  AkinatorError StoreDB();
  AkinatorError GuessMode();
  AkinatorError DefineMode();
  AkinatorError CompareMode();
  AkinatorError ShowTreeMode();
  AkinatorError QuitAndSaveMode();
  AkinatorError QuitWithoutMode();

  BTree data_base_;
  String raw_data_base_;
  const char* db_file_name_;
};

Mode GetMode();

#endif // AKINATOR_H_
