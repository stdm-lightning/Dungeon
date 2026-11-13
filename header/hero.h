#ifndef HERO_H
#define HERO_H

#include "creature.h"

class Map;

class Hero : public Creature {
private:
    int gold;

public:
    Hero();
    Hero(int startX, int startY, int startHp);
    ~Hero() override;

    // Сигнатура полностью совпадает с Creature::make_move
    void make_move(const Map& map, int heroX, int heroY, std::vector<std::unique_ptr<Creature>>& allMonsters, Hero& hero) override;

    bool move(int dx, int dy, Map& map, std::vector<std::unique_ptr<Creature>>& monsters);
    void printStatus() const;
    int getGold() const { return gold; }
    void addGold(int amount) { gold += amount; }
};

void inspectCurrentTile(const Hero& hero, const Map& map);

#endif // HERO_H