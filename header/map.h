#ifndef MAP_H
#define MAP_H

#include <string>

class Map {
private:
    int width;
    int height;
    char* cells;
public:
    int startHeroX = 1;
    int startHeroY = 1;
    
    Map(int w, int h);

    ~Map();
    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;

    Map(Map&& other) ;
    Map& operator=(Map&& other);

    bool isInside(int x, int y) const;
    char getCell(int x, int y) const;
    void setCell(int x, int y, char c);
    bool isWalkable(int x, int y) const;
    void render(int heroX, int heroY) const;

    void initDefaultRoom();

    static Map loadFromFile(const std::string& filename);
};


#endif // MAP_H