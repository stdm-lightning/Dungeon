#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>
#include <memory>

class Creature;

class Map {
private:
    int width;
    int height;
    char* cells;

    // ИНВАРИАНТ КЛАССА Map: width > 0, height > 0, cells хранит валидный массив размером width * height, координаты в пределах сетки.

    void validateCoords(int x, int y) const;

public:
    int startHeroX = 1;
    int startHeroY = 1;

    Map(int w, int h);
    ~Map();

    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;

    Map(Map&& other) noexcept;
    Map& operator=(Map&& other) noexcept;

    int getWidth() const;
    int getHeight() const;

    bool isInside(int x, int y) const;
    char getCell(int x, int y) const;
    void setCell(int x, int y, char c);
    bool isWalkable(int x, int y) const;

    void render(int heroX, int heroY, const std::vector<std::unique_ptr<Creature>>& monsters) const;

    void initDefaultRoom();

    static Map loadFromFile(const std::string& filename);
};

class MapRenderer {
public:
    static void render(const Map& map, int heroX, int heroY, const std::vector<std::unique_ptr<Creature>>& monsters);
};

#endif // MAP_H