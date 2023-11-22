#include <locale.h>

#include "akinatorcode/include/akinator.h"

int main(int argc, const char** argv) {
  setlocale(LC_ALL, "ru_RU.UTF8");

  AkinatorError error = AkinatorError::kSuccess;

  Akinator akinator = {};
  error = akinator.Start(argc, argv);

  while (error == AkinatorError::kSuccess) {
    Mode mode = GetMode();

    error = akinator.ExecuteMode(mode);
    if (mode == Mode::kQuitAndSave
        || mode == Mode::kQuitWithoutSave) {
      break;
    }
  }

  akinator.ThrowError(error);
  akinator.End();

  return 0;
}
