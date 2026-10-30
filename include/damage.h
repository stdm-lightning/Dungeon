#ifndef DUNGEON_DAMAGE_H
#define DUNGEON_DAMAGE_H
#include <stdbool.h>
#include "stata.h"
bool t_damage(int *p_hp, int damage);
void handle_monster_combat(GameState *game, int monster_index);
#endif //DUNGEON_DAMAGE_H
