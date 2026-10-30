#include "run.h"
#include <windows.h>
int main() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    run_game();
    return 0;
}

