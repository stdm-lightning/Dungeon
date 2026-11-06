#include <iostream>
#include <windows.h>
#include "run.h"

int main() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    runGame();

    std::cout << "\nИгра завершена. Нажмите Enter для выхода...";
    std::cin.get();
    return 0;
}