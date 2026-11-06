#include "hero.h"
#include "map.h"
#include <iostream>

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
    char cell = map.getCell(hero.x, hero.y);
    if (cell == '$') {
        std::cout << "Под ногами блестит золото!\n";
    }
}

bool Hero::move(int dx, int dy, Map& map) {
    int targetX = this->x + dx;
    int targetY = this->y + dy;

    // Герой спрашивает у карты, проходима ли целевая клетка
    if (!map.isWalkable(targetX, targetY)) {
        std::cout << "Там стена или край подземелья! Ход не потрачен.\n";
        return false;
    }

    // Изменяем собственные координаты через указатель this
    this->x = targetX;
    this->y = targetY;

    // Явное использование `this`: передаём сам объект (*this) во внешнюю функцию
    inspectCurrentTile(*this, map);

    // Если на клетке золото — подбираем его и очищаем клетку карты
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