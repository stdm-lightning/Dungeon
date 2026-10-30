#include "monster.h"
#include <stdlib.h>
#include "stata.h"
void add_monster(GameState *game, int x, int y, int hp, int damage, char symbol) {
    if (game == NULL) return;

    if (game->monster_count >= game->monster_capacity) {
        game->monster_capacity = (game->monster_capacity == 0) ? 4 : game->monster_capacity * 2;
        Monster *temp = (Monster *)realloc(game->monsters, (size_t)game->monster_capacity * sizeof(Monster));
        if (temp != NULL) {
            game->monsters = temp;
        } else {
            return; // Ошибка выделения памяти
        }
    }
    game->monsters[game->monster_count].x = x;
    game->monsters[game->monster_count].y = y;
    game->monsters[game->monster_count].hp = hp;
    game->monsters[game->monster_count].damage = damage;
    game->monsters[game->monster_count].type_symbol = symbol;
    game->monster_count++;
}

void remove_monster(GameState *game, int index) {
    if (game == NULL || index < 0 || index >= game->monster_count) return;
    for (int i = index; i < game->monster_count - 1; i++) {
        game->monsters[i] = game->monsters[i + 1];
    }
    game->monster_count--;
}