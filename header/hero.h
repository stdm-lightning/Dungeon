#ifndef HERO_H
#define HERO_H

class Map;

class Hero {
public:
    int x;
    int y;
    int hp;
    int maxHp;
    int gold;

    Hero();

    Hero(int startX, int startY, int startHp);

    ~Hero();

    bool move(int dx, int dy, Map& map);
    void takeDamage(int amount);
    void printStatus() const;
};

void inspectCurrentTile(const Hero& hero, const Map& map);

#endif // HERO_H