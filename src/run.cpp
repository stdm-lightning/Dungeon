#include <iostream>
#include <string>
#include <sstream>
#include "map.h"
#include "hero.h"

void demonstrateObjectLifetime() {
    std::cout << "=== ДЕМОНСТРАЦИЯ ВРЕМЕНИ ЖИЗНИ ОБЪЕКТОВ (Практика 10) ===\n";

    std::cout << "1. Создаём объект карты на стеке (stackMap):\n";
    Map stackMap(5, 5);

    std::cout << "\n2. Создаём объект карты в куче (heapMap):\n";
    Map* heapMap = new Map(6, 6);

    std::cout << "\n3. Входим во вложенный блок { ... }:\n";
    {
        Map blockMap(4, 4);
        Hero defaultHero; // Проверка конструктора героя по умолчанию
        std::cout << "   Завершаем работу внутри вложенного блока...\n";
    } // Здесь автоматически вызовутся ~Hero() и ~Map(4x4) в обратном порядке!
    std::cout << "4. Вышли из вложенного блока.\n";

    std::cout << "\n5. Явно удаляем объект в куче (delete heapMap):\n";
    delete heapMap;
    heapMap = nullptr;

    std::cout << "\n6. Завершаем функцию demonstrateObjectLifetime() (сейчас уничтожится stackMap):\n";
}

void runGame() {
    // 1. Демонстрация конструкторов, деструкторов, стека, кучи и блоков
    demonstrateObjectLifetime();

    std::cout << "\n=== ПРОВЕРКА БЕЗОПАСНОЙ ЗАГРУЗКИ ПРИ ОШИБКЕ В ФАЙЛЕ ===\n";
    try {
        Map brokenMap = Map::loadFromFile("non_existent_level.txt");
    } catch (const std::exception& e) {
        std::cout << "Перехвачена ошибка загрузки: " << e.what()
                  << " (утечек памяти нет)\n";
    }

    std::cout << "\n=== ЗАПУСК ИГРЫ «ПОДЗЕМЕЛЬЕ» (C++17) ===\n";

    // Загружаем карту из level.txt, а если файла нет — создаём стандартную комнату 10x6
    Map gameMap = [&]() {
        try {
            std::cout << "Попытка загрузки уровня из файла 'level.txt'...\n";
            return Map::loadFromFile("level.txt");
        } catch (const std::exception& e) {
            std::cout << "Не удалось загрузить 'level.txt' (" << e.what()
                      << "). Создаём базовую карту 10x6.\n";
            Map fallback(10, 6);
            fallback.initDefaultRoom();
            return fallback;
        }
    }();

    // Создаём героя через конструктор с параметрами
    gameMap = [&]() {
        try {
            return Map::loadFromFile("level.txt");
        } catch (...) {
            Map fallback(10, 6);
            fallback.initDefaultRoom();
            fallback.startHeroX = 1; // Дефолтные координаты для запасной комнаты
            fallback.startHeroY = 1;
            return fallback;
        }
    }();
    // Создаем героя в той точке, которая была указана в level.txt в символе '@'
    Hero hero(gameMap.startHeroX, gameMap.startHeroY, 100);

    int turnCount = 1;
    bool isRunning = true;

    std::cout << "\nДоступные команды: north (n), south (s), east (e), west (w), "
              << "look, status, hit N, quit (q)\n\n";

    while (isRunning) {
        gameMap.render(hero.x, hero.y);
        std::cout << "\n[Ход " << turnCount << "] Введите команду > ";

        std::string line;
        if (!std::getline(std::cin, line)) {
            break;
        }

        std::istringstream iss(line);
        std::string cmd;
        if (!(iss >> cmd)) {
            continue;
        }

        for (char& c : cmd) {
            if (c >= 'A' && c <= 'Z') {
                c = c - 'A' + 'a';
            }
        }

        if (cmd == "go") {
            if (!(iss >> cmd)) {
                std::cout << "Укажите направление после 'go' (например: go north).\n";
                continue;
            }
            for (char& c : cmd) {
                if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
            }
        }

        if (cmd == "quit" || cmd == "q") {
            std::cout << "Выход из игры...\n";
            isRunning = false;
        } else if (cmd == "north" || cmd == "n") {
            if (hero.move(0, -1, gameMap)) turnCount++;
        } else if (cmd == "south" || cmd == "s") {
            if (hero.move(0, 1, gameMap)) turnCount++;
        } else if (cmd == "east" || cmd == "e") {
            if (hero.move(1, 0, gameMap)) turnCount++;
        } else if (cmd == "west" || cmd == "w") {
            if (hero.move(-1, 0, gameMap)) turnCount++;
        } else if (cmd == "look" || cmd == "l") {
            std::cout << "Герой осматривается вокруг.\n";
            inspectCurrentTile(hero, gameMap);
        } else if (cmd == "status") {
            hero.printStatus();
        } else if (cmd == "hit") {
            int dmg = 0;
            if (iss >> dmg && dmg >= 0) {
                hero.takeDamage(dmg);
                turnCount++;
            } else {
                std::cout << "Ошибка: укажите неотрицательное число урона (например: hit 15).\n";
            }
        } else {
            std::cout << "Неизвестная команда. Подсказка: north, south, east, west, look, status, hit N, quit.\n";
        }

        if (hero.hp <= 0) {
            std::cout << "\n========================================\n";
            std::cout << "        ВЫ МЕРТВЫ! ИГРА ОКОНЧЕНА.       \n";
            std::cout << "========================================\n";
            isRunning = false; // Выход из игрового цикла
        }

        std::cout << "----------------------------------------\n";
    }

    std::cout << "Завершение main(). Сейчас автоматически вызовутся деструкторы объектов на стеке:\n";
}