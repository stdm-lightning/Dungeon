#include "creature.h"
#include "map.h"
#include "hero.h"
#include <iostream>

static bool isCellFree(int targetX, int targetY, int heroX, int heroY, const std::vector<std::unique_ptr<Creature>>& allMonsters) {
    if (targetX == heroX && targetY == heroY) return false;
    for (const auto& m : allMonsters) {
        if (m && m->isAlive() && m->getX() == targetX && m->getY() == targetY) {
            return false;
        }
    }
    return true;
}

void AggressiveMonster::make_move(const Map& map, int heroX, int heroY, std::vector<std::unique_ptr<Creature>>& allMonsters, Hero& hero) {
    int dx = 0;
    int dy = 0;
    if (heroX > x) dx = 1;
    else if (heroX < x) dx = -1;

    int targetX = x + dx;
    int targetY = y;

    if (targetX == heroX && targetY == heroY) {
        std::cout << "[БОЙ] Агрессивный монстр (A) атакует героя и наносит 8 ед. урона!\n";
        hero.takeDamage(8);
        return;
    }

    if (dx != 0 && map.isWalkable(targetX, targetY) && isCellFree(targetX, targetY, heroX, heroY, allMonsters)) {
        x = targetX;
        return;
    }

    if (heroY > y) dy = 1;
    else if (heroY < y) dy = -1;

    targetX = x;
    targetY = y + dy;

    if (targetX == heroX && targetY == heroY) {
        std::cout << "[БОЙ] Агрессивный монстр (A) атакует героя и наносит 8 ед. урона!\n";
        hero.takeDamage(8);
        return;
    }

    if (dy != 0 && map.isWalkable(targetX, targetY) && isCellFree(targetX, targetY, heroX, heroY, allMonsters)) {
        y = targetY;
    }
}

void RoamingMonster::make_move(const Map& map, int heroX, int heroY, std::vector<std::unique_ptr<Creature>>& allMonsters, Hero& hero) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(-1, 1);

    int dx = dis(gen);
    int dy = dis(gen);

    int targetX = x + dx;
    int targetY = y + dy;
    if (targetX == heroX && targetY == heroY) {
        std::cout << "[БОЙ] Блуждающий монстр (R) атаковал героя и наносит 8 ед. урона!\n";
        hero.takeDamage(8);


        int escapeDx = dis(gen);
        int escapeDy = dis(gen);
        int escapeX = x + escapeDx;
        int escapeY = y + escapeDy;
        if (map.isWalkable(escapeX, escapeY) && isCellFree(escapeX, escapeY, heroX, heroY, allMonsters)) {
            x = escapeX;
            y = escapeY;
            std::cout << "[МАНЕВР] Монстр (R) отпрыгнул в сторону после атаки.\n";
        }
        return;
    }

    if (map.isWalkable(targetX, targetY) && isCellFree(targetX, targetY, heroX, heroY, allMonsters)) {
        x = targetX;
        y = targetY;
    }
}