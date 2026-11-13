#include "map.h"
#include "creature.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

Map::Map(int w, int h) : width(w), height(h), cells(nullptr) {
    if (w <= 0 || h <= 0) {
        throw std::invalid_argument("Ошибка: недопустимые размеры карты.");
    }
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

int Map::getWidth() const { return width; }
int Map::getHeight() const { return height; }

void Map::validateCoords(int x, int y) const {
    if (!isInside(x, y)) {
        throw std::out_of_range("Координаты выходят за пределы карты!");
    }
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
    if (!isInside(x, y)) {
        return false;
    }
    return getCell(x, y) != '#';
}

void Map::render(int heroX, int heroY, const std::vector<std::unique_ptr<Creature>>& monsters) const {
    MapRenderer::render(*this, heroX, heroY, monsters);
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
        throw std::runtime_error("повреждённый формат заголовка.");
    }

    if (w <= 2 || h <= 2 || w > 200 || h > 200) {
        throw std::runtime_error("недопустимый размер карты.");
    }

    std::string dummy;
    std::getline(file, dummy);

    Map loadedMap(w, h);
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
                loadedMap.startHeroX = x;
                loadedMap.startHeroY = y;
                loadedMap.setCell(x, y, '.');
            } else {
                loadedMap.setCell(x, y, c);
            }
        }
    }

    return loadedMap;
}

void MapRenderer::render(const Map& map, int heroX, int heroY, const std::vector<std::unique_ptr<Creature>>& monsters) {
    int h = map.getHeight();
    int w = map.getWidth();

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (x == heroX && y == heroY) {
                std::cout << '@';
                continue;
            }

            char symbolToPrint = map.getCell(x, y);
            bool monsterFound = false;

            for (const auto& m : monsters) {
                if (m && m->getX() == x && m->getY() == y && m->isAlive()) {
                    symbolToPrint = m->getSymbol();
                    monsterFound = true;
                    break;
                }
            }

            std::cout << symbolToPrint;
        }
        std::cout << '\n';
    }
}