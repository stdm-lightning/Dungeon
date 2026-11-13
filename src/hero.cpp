#include "hero.h"
#include "map.h"
#include <iostream>
#include <random>

Hero::Hero() : Creature(1, 1, 100, '@'), gold(0) {
    std::cout << "[RAII] Конструктор по умолчанию Hero() по адресу " << this << "\n";
}

Hero::Hero(int startX, int startY, int startHp)
    : Creature(startX, startY, startHp, '@'), gold(0) {
    std::cout << "[RAII] Конструктор с параметрами Hero(" << x << ", " << y
              << ", HP=" << hp << ") по адресу " << this << "\n";
}

Hero::~Hero() {
    std::cout << "[RAII] Деструктор ~Hero() по адресу " << this << "\n";
}

void Hero::make_move(const Map& /*map*/, int /*heroX*/, int /*heroY*/, std::vector<std::unique_ptr<Creature>>& /*allMonsters*/, Hero& /*hero*/) {}

void inspectCurrentTile(const Hero& hero, const Map& map) {
    char cell = map.getCell(hero.getX(), hero.getY());
    if (cell == '$') {
        std::cout << "Под ногами блестит золото!\n";
    }
}

bool Hero::move(int dx, int dy, Map& map, std::vector<std::unique_ptr<Creature>>& monsters) {
    int targetX = this->x + dx;
    int targetY = this->y + dy;

    for (auto& m : monsters) {
        if (m && m->isAlive() && m->getX() == targetX && m->getY() == targetY) {
            std::cout << "[БОЙ] Герой атаковал монстра (" << m->getSymbol() << ") и нанес 15 ед. урона!\n";
            m->takeDamage(15);

            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(1, 100);
            int chance = dist(gen); // от 1 до 100

            if (chance <= 25) {
                std::cout << "[СТОЛКНОВЕНИЕ] Герой неудачно столкнулся с монстром при ударе и получил 8 ед. урона!\n";
                this->takeDamage(8);
            }

            if (!m->isAlive()) {
                std::cout << "[ПОБЕДА] Монстр (" << m->getSymbol() << ") повержен!\n";
            } else {
                std::cout << "У здоровья монстра осталось: " << m->getHp() << " ХП.\n";
            }
            return true; // Ход потрачен на атаку
        }
    }

    if (!map.isWalkable(targetX, targetY)) {
        std::cout << "Там стена или край подземелья! Ход не потрачен.\n";
        return false;
    }

    this->x = targetX;
    this->y = targetY;

    inspectCurrentTile(*this, map);

    if (map.getCell(this->x, this->y) == '$') {
        this->gold += 10;
        map.setCell(this->x, this->y, '.');
        std::cout << "Вы подобрали 10 золотых! Всего золота: " << this->gold << "\n";
    }

    return true;
}

void Hero::printStatus() const {
    double hpPercent = (static_cast<double>(this->hp) / maxHp) * 100.0;
    std::cout << "[Статус] Координаты: (" << this->x << ", " << this->y << ") | "
              << "HP: " << this->hp << "/" << this->maxHp << " (" << hpPercent << "%) | "
              << "Золото: " << this->gold << "\n";
}