#ifndef MAP_H
#define MAP_H

#include <string>

class Map {
public:
    int width;
    int height;
    char* cells;

    int startHeroX = 1;
    int startHeroY = 1;
    
    Map(int w, int h);

    ~Map();

    // Запрет копирования (Подсказка из Практики 10):
    // Если класс владеет сырым указателем, копирование по умолчанию скопирует
    // только адрес, и деструкторы двух объектов освободят один блок дважды (double free).
    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;

    Map(Map&& other) noexcept;
    Map& operator=(Map&& other) noexcept;

    bool isInside(int x, int y) const;
    char getCell(int x, int y) const;
    void setCell(int x, int y, char c);
    bool isWalkable(int x, int y) const;
    void render(int heroX, int heroY) const;

    // Загрузка базовой комнаты по умолчанию
    void initDefaultRoom();

    // При ошибке в файле бросает исключение — объект карты не создается,
    // а любая уже выделенная память гарантированно освобождается деструктором.
    static Map loadFromFile(const std::string& filename);
};

#endif // MAP_H