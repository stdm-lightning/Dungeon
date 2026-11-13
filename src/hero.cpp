#include "hero.h"
#include "map.h"
#include <iostream>
int Hero::getX() const {
    return this->x;
}
int Hero::getY() const {
    return this->y;
}
int Hero::getHp() const {
    return this->hp;
}
Hero::Hero() : x(1), y(1), hp(100), maxHp(100), gold(0) {
    std::cout << "[RAII] Конструктор по умолчанию Hero() по адресу " << this << "\n";
}

Hero::Hero(int startX, int startY, int startHp) 
    : x(startX), y(startY), hp(startHp), maxHp(startHp), gold(0) {
    std::cout << "[RAII] Конструктор с параметрами Hero(" << x << ", " << y 
              << ", HP=" << hp << ") по адресу " << this << "\n";
}

Hero::~Hero() {
    std::cout << "[RAII] Деструктор ~Hero() по адресу " << this << "\n";
}

void inspectCurrentTile(const Hero& hero, const Map& map) {
    char cell = map.getCell(hero.getX(), hero.getY() );
    if (cell == '$') {
        std::cout << "Под ногами блестит золото!\n";
    }
}

bool Hero::move(int dx, int dy, Map& map) {
    int targetX = this->x + dx;
    int targetY = this->y + dy;

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

void Hero::takeDamage(int amount) {
    if (amount <= 0) return;
    this->hp -= amount;
    if (this->hp < 0) {
        this->hp = 0;
    }
    std::cout << "Герой получил " << amount << " ед. урона. Здоровье: " 
              << this->hp << "/" << this->maxHp << "\n";
}

void Hero::printStatus() const {
    double hpPercent = (static_cast<double>(this->hp) / this->maxHp) * 100.0;
    std::cout << "[Статус] Координаты: (" << this->x << ", " << this->y << ") | "
              << "HP: " << this->hp << "/" << this->maxHp << " (" << hpPercent << "%) | "
              << "Золото: " << this->gold << "\n";
}