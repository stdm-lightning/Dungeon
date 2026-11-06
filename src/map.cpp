#include "map.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>

Map::Map(int w, int h) : width(w), height(h), cells(nullptr) {
    if (w <= 0 || h <= 0) {
        throw std::invalid_argument("Ошибка: недопустимые размеры карты.");
    }
    // Выделяем память в конструкторе (RAII)
    cells = new char[width * height];
    for (int i = 0; i < width * height; ++i) {
        cells[i] = '.';
    }
    std::cout << "[RAII] Конструктор Map(" << width << "x" << height 
              << ") по адресу " << this << "\n";
}

Map::~Map() {
    std::cout << "[RAII] Деструктор ~Map(" << width << "x" << height 
              << ") по адресу " << this << "\n";
    // Единственное место освобождения памяти — деструктор
    delete[] cells;
    cells = nullptr;
}

Map::Map(Map&& other) noexcept 
    : width(other.width), height(other.height), cells(other.cells) {
    other.width = 0;
    other.height = 0;
    other.cells = nullptr;
}

Map& Map::operator=(Map&& other) noexcept {
    if (this != &other) {
        delete[] cells;
        width = other.width;
        height = other.height;
        cells = other.cells;
        other.width = 0;
        other.height = 0;
        other.cells = nullptr;
    }
    return *this;
}

bool Map::isInside(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

char Map::getCell(int x, int y) const {
    if (!isInside(x, y)) {
        return '#';
    }
    return cells[y * width + x];
}

void Map::setCell(int x, int y, char c) {
    if (isInside(x, y)) {
        cells[y * width + x] = c;
    }
}

bool Map::isWalkable(int x, int y) const {
    // Сначала проверяем границы, затем содержимое клетки
    if (!isInside(x, y)) {
        return false;
    }
    return getCell(x, y) != '#';
}

void Map::render(int heroX, int heroY) const {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (x == heroX && y == heroY) {
                std::cout << '@';
            } else {
                std::cout << getCell(x, y);
            }
        }
        std::cout << '\n';
    }
}

void Map::initDefaultRoom() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (y == 0 || y == height - 1 || x == 0 || x == width - 1) {
                setCell(x, y, '#');
            } else {
                setCell(x, y, '.');
            }
        }
    }
    // Внутренние стены и золото
    if (width >= 8 && height >= 5) {
        setCell(3, 2, '#');
        setCell(4, 2, '#');
        setCell(2, 3, '$');
        setCell(6, 1, '$');
    }
}

Map Map::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("файл не найден (" + filename + ").");
    }

    int w = 0, h = 0;
    if (!(file >> w >> h)) {
        throw std::runtime_error("повреждённый формат заголовка (ожидались ширина и высота).");
    }

    if (w <= 2 || h <= 2 || w > 200 || h > 200) {
        throw std::runtime_error("недопустимый размер карты.");
    }

    std::string dummy;
    std::getline(file, dummy); // Считываем остаток первой строки

    // Создаём объект карты на стеке.
    // ВАЖНО (RAII): Память под клетки уже выделена в конструкторе `loadedMap`.
    // Если ниже в цикле обнаружится ошибка в файле и будет выброшено исключение,
    // при раскрутке стека автоматически вызовется деструктор `~Map()`,
    // который освободит `cells`. Утечки памяти не произойдет ни на одном пути!
    Map loadedMap(w, h);
    bool heroFound = false;
    for (int y = 0; y < h; ++y) {
        std::string line;
        if (!std::getline(file, line)) {
            throw std::runtime_error("неожиданный конец файла.");
        }
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        for (int x = 0; x < w; ++x) {
            char c = line[x];
            if (c == '@') {
                // Запоминаем, где нашли героя в файле
                loadedMap.startHeroX = x;
                loadedMap.startHeroY = y;
                loadedMap.setCell(x, y, '.'); // На карту ставим пустой пол вместо @
                heroFound = true;
            } else {
                loadedMap.setCell(x, y, c);
            }
        }
    }

    return loadedMap;
}