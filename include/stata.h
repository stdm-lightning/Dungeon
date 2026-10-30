#ifndef DUNGEON_STATA_H
#define DUNGEON_STATA_H

#define invisible (1U << 0)
#define poisoned  (1U << 1)
#define blinded   (1U << 2)
#define frozen   (1U << 3)
#define burned   (1U << 4)
#define sleeping (1U << 5)
extern int hero_y;
extern int hero_x;

typedef enum {
    OK = 0,
    ERR_FILE_NOT_FOUND,
    ERR_CORRUPTED_FORMAT,
    ERR_OUT_OF_MEMORY,
    ERR_INVALID_SIZE
} GameError;

typedef struct {
    char name[32];
    int stat_bonus;
    unsigned int effect_flag;
    int duration;
} Item;

typedef struct {
    char name[32];
    int turns_left;
    int stat_bonus;
    unsigned int effect_flag;
} Buff;

typedef struct {
    int y;
    int x;
    int hp; // целое знаковое число, чтобы если например урон был больше чем здоровье, то не случиось переполнение при расчете здоровья
    int max_hp;
    int damage;
    unsigned int gold; // золото и этаж не могут быть отрицательными
    unsigned int depth;
    unsigned int status; // выбрано по условию
    Item inventory[10];
    int inventory_count;
    Buff buffs[5];
    int buff_count;
} Hero;

typedef struct {
    char *cells;
    int width;
    int height;
} Map;

typedef struct {
    int y;
    int x;
    int hp;
    int damage;
    char type_symbol;
} Monster;

typedef struct {
    Map map;
    Hero hero;
    Monster *monsters;
    int monster_count;
    int monster_capacity;
} GameState;

void p_status(const Hero *hero);
void update_buffs(Hero *hero);
#endif //DUNGEON_STATA_H
