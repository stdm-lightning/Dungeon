#ifndef DUNGEON_MAP_H
#define DUNGEON_MAP_H
#include "stata.h"
#include <stdbool.h>
GameError random_map(GameState *game, int width, int height);
bool save_map(const char *filename, const GameState *game);
GameError load_map(const char *filename, GameState *game);

bool save_game(const char *filename, const GameState *game);
GameError load_game(const char *filename, GameState *game);

void free_game_state(GameState *game);

char get_cell(const Map *map, int x, int y);
void set_cell(Map *map, int x, int y, char symbol);

void p_map(const GameState *game);
bool c_map(const char *validcmd, GameState *game);

bool addr(const Map *map, int *p_x, int *p_y, const char *validcmd);
void p_check(const GameState *game);

#endif
