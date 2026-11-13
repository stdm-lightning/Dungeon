#include <iostream>
#include "run.h"
#ifdef _WIN32
#include <windows.h>
#endif
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    runGame();

    std::cout << "\nИгра завершена. Нажмите Enter для выхода...";
    std::cin.get();
    return 0;
}