#ifndef CREATURE_H
#define CREATURE_H

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <random>

class Map;

class Creature {
protected:
    int x;
    int y;
    int hp;
    int maxHp;
    char symbol;

    // ИНВАРИАНТ КЛАССА Creature: здоровье hp всегда находится в диапазоне [0, maxHp].

    // Статический счетчик создан для отслеживания числа существ на уровне в единственном экземпляре на весь класс.
    inline static int monsterCount = 0;

public:
    Creature(int startX, int startY, int startHp, char sym)
        : x(startX), y(startY), hp(startHp), maxHp(startHp), symbol(sym) {}


    virtual ~Creature() = default;

    virtual void make_move(const Map& map, int heroX, int heroY, std::vector<std::unique_ptr<Creature>>& allMonsters, class Hero& hero) = 0;

    int getX() const { return x; }
    int getY() const { return y; }
    int getHp() const { return hp; }
    char getSymbol() const { return symbol; }
    bool isAlive() const { return hp > 0; }

    void takeDamage(int amount) {
        if (amount <= 0) return;
        hp -= amount;
        if (hp < 0) hp = 0;
    }

    static int getMonsterCount() { return monsterCount; }
};

class AggressiveMonster : public Creature {
public:
    AggressiveMonster(int startX, int startY) : Creature(startX, startY, 100, 'A') { monsterCount++; }
    ~AggressiveMonster() override { monsterCount--; }

    void make_move(const Map& map, int heroX, int heroY, std::vector<std::unique_ptr<Creature>>& allMonsters, Hero& hero) override;
};

class StaticMonster : public Creature {
public:
    StaticMonster(int startX, int startY) : Creature(startX, startY, 100, 'S') { monsterCount++; }
    ~StaticMonster() override { monsterCount--; }

    void make_move(const Map& /*map*/, int /*heroX*/, int /*heroY*/, std::vector<std::unique_ptr<Creature>>& /*allMonsters*/, Hero& /*hero*/) override {}
};

class RoamingMonster : public Creature {
public:
    RoamingMonster(int startX, int startY) : Creature(startX, startY, 100, 'R') { monsterCount++; }
    ~RoamingMonster() override { monsterCount--; }

    void make_move(const Map& map, int heroX, int heroY, std::vector<std::unique_ptr<Creature>>& allMonsters, Hero& hero) override;
};

#endif // CREATURE_H