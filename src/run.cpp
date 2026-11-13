#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include "map.h"
#include "hero.h"
#include "creature.h"

void demonstrateObjectLifetime() {
    std::cout << "=== ДЕМОНСТРАЦИЯ ВРЕМЕНИ ЖИЗНИ ОБЪЕКТОВ (Практика 10) ===\n";

    std::cout << "1. Создаём объект карты на стеке (stackMap):\n";
    Map stackMap(5, 5);

    std::cout << "\n2. Создаём объект карты в куче (heapMap):\n";
    Map* heapMap = new Map(6, 6);

    std::cout << "\n3. Входим во вложенный блок { ... }:\n";
    {
        Map blockMap(4, 4);
        Hero defaultHero;
        std::cout << "   Завершаем работу внутри вложенного блока...\n";
    }
    std::cout << "4. Вышли из вложенного блока.\n";

    std::cout << "\n5. Явно удаляем объект в куче (delete heapMap):\n";
    delete heapMap;
    heapMap = nullptr;

    std::cout << "\n6. Завершаем функцию demonstrateObjectLifetime().\n";
}

void runGame() {
    demonstrateObjectLifetime();

    std::cout << "\n=== ПРОВЕРКА БЕЗОПАСНОЙ ЗАГРУЗКИ ПРИ ОШИБКЕ В ФАЙЛЕ ===\n";
    try {
        Map brokenMap = Map::loadFromFile("non_existent_level.txt");
    } catch (const std::exception& e) {
        std::cout << "Перехвачена ошибка загрузки: " << e.what()
                  << " (утечек памяти нет)\n";
    }

    std::cout << "\n=== ЗАПУСК ИГРЫ «ПОДЗЕМЕЛЬЕ» (C++17, Практики 11-12) ===\n";

    Map gameMap = [&]() {
        try {
            return Map::loadFromFile("level.txt");
        } catch (...) {
            Map fallback(10, 6);
            fallback.initDefaultRoom();
            fallback.startHeroX = 1;
            fallback.startHeroY = 1;
            return fallback;
        }
    }();

    Hero hero(gameMap.startHeroX, gameMap.startHeroY, 100);

    std::vector<std::unique_ptr<Creature>> monsters;
    monsters.push_back(std::make_unique<AggressiveMonster>(2, 1));
    monsters.push_back(std::make_unique<StaticMonster>(4, 4));
    monsters.push_back(std::make_unique<RoamingMonster>(7, 2));

    std::cout << "Всего создано монстров на уровне: " << Creature::getMonsterCount() << "\n\n";

    int turnCount = 1;
    bool isRunning = true;

    std::cout << "Доступные команды: north (n), south (s), east (e), west (w), "
              << "look, status, hit N, quit (q)\n\n";

    while (isRunning) {
        MapRenderer::render(gameMap, hero.getX(), hero.getY(), monsters);

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
                std::cout << "Укажите направление после 'go'.\n";
                continue;
            }
            for (char& c : cmd) {
                if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
            }
        }

        bool heroMoved = false;

        if (cmd == "quit" || cmd == "q") {
            std::cout << "Выход из игры...\n";
            isRunning = false;
        } else if (cmd == "north" || cmd == "n") {
            if (hero.move(0, -1, gameMap, monsters)) { heroMoved = true; turnCount++; }
        } else if (cmd == "south" || cmd == "s") {
            if (hero.move(0, 1, gameMap, monsters)) { heroMoved = true; turnCount++; }
        } else if (cmd == "east" || cmd == "e") {
            if (hero.move(1, 0, gameMap, monsters)) { heroMoved = true; turnCount++; }
        } else if (cmd == "west" || cmd == "w") {
            if (hero.move(-1, 0, gameMap, monsters)) { heroMoved = true; turnCount++; }
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
                std::cout << "Ошибка: укажите неотрицательное число урона.\n";
            }
        } else {
            std::cout << "Неизвестная команда.\n";
        }

        if (heroMoved) {
            for (const auto& monster : monsters) {
                if (monster && monster->isAlive()) {
                    monster->make_move(gameMap, hero.getX(), hero.getY(), monsters, hero);
                }
            }
        }

        if (hero.getHp() <= 0) {
            std::cout << "\n========================================\n";
            std::cout << "        ВЫ МЕРТВЫ! ИГРА ОКОНЧЕНА.       \n";
            std::cout << "========================================\n";
            isRunning = false;
        }

        std::cout << "----------------------------------------\n";
    }

    std::cout << "Завершение main(). Память монстров и карты очистится автоматически через RAII.\n";
}