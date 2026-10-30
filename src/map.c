#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "stata.h"
#include "damage.h"
#include "monster.h"

extern int m_count;
static bool dfs(const Map *map, int y, int x, bool *visited) {
    if (x < 0 || x >= map->width || y < 0 || y >= map->height) return false;
    int idx = y * map->width + x;
    char sym = get_cell(map, x, y);
    if (sym == '#' || visited[idx]) return false;
    if (sym == 'E') return true;

    visited[idx] = true;
    if (dfs(map, y + 1, x, visited)) return true;
    if (dfs(map, y - 1, x, visited)) return true;
    if (dfs(map, y, x + 1, visited)) return true;
    if (dfs(map, y, x - 1, visited)) return true;
    return false;
}
void free_game_state(GameState *game) {
    if (game == NULL) return;
    if (game->map.cells != NULL) {
        free(game->map.cells);
        game->map.cells = NULL;
    }
    if (game->monsters != NULL) {
        free(game->monsters);
        game->monsters = NULL;
    }
}
char get_cell(const Map *map, int x, int y) {
    if (map == NULL || x < 0 || x >= map->width || y < 0 || y >= map->height) {
        return '#';
    }
    return map->cells[y * map->width + x];
}

void set_cell(Map *map, int x, int y, char symbol) {
    if (map == NULL || x < 0 || x >= map->width || y < 0 || y >= map->height) {
        return;
    }
    map->cells[y * map->width + x] = symbol;
}

// Рекурсивная проверка достижимости выхода 'E' (Практика 4)
static bool check_path_exists(const Map *map, int start_y, int start_x) {
    size_t visited_size = (size_t)map->width * (size_t)map->height * sizeof(bool);
    bool *visited = (bool *)malloc(visited_size);
    if (visited == NULL) return false;

    for (int i = 0; i < map->width * map->height; i++) {
        visited[i] = false;
    }

    bool result = dfs(map, start_y, start_x, visited);
    free(visited);
    return result;
}

bool save_map(const char *filename, const GameState *game) {
    if (filename == NULL || game == NULL || game->map.cells == NULL) return false;

    FILE *file = fopen(filename, "w");
    if (file == NULL) return false;

    for (int y = 0; y < game->map.height; y++) {
        for (int x = 0; x < game->map.width; x++) {
            char ch = get_cell(&game->map, x, y);
            if (y == game->hero.y && x == game->hero.x) {
                ch = 'P';
            } else {
                for (int m = 0; m < game->monster_count; m++) {
                    if (game->monsters[m].x == x && game->monsters[m].y == y && game->monsters[m].hp > 0) {
                        ch = game->monsters[m].type_symbol; // 'M'
                        break;
                    }
                }
            }
            fputc(ch, file);
        }
        fputc('\n', file);
    }

    fclose(file);
    return true;
}

GameError load_map(const char *filename, GameState *game) {
    if (filename == NULL || game == NULL) return ERR_FILE_NOT_FOUND;

    FILE *file = fopen(filename, "r");
    if (file == NULL) return ERR_FILE_NOT_FOUND;

    int width = 0;
    int height = 0;
    char buffer[512];

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        int len = 0;
        while (buffer[len] != '\0' && buffer[len] != '\r' && buffer[len] != '\n') {
            len++;
        }
        if (len > 0) {
            if (width == 0) {
                width = len;
            } else if (width != len) {
                fclose(file);
                return ERR_CORRUPTED_FORMAT;
            }
            height++;
        }
    }

    if (width <= 0 || height <= 0) {
        fclose(file);
        return ERR_INVALID_SIZE;
    }

    rewind(file);

    game->map.width = width;
    game->map.height = height;
    game->map.cells = (char *)malloc((size_t)width * (size_t)height * sizeof(char));
    if (game->map.cells == NULL) {
        fclose(file);
        return ERR_OUT_OF_MEMORY;
    }

    int y = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL && y < height) {
        int x = 0;
        while (buffer[x] != '\0' && buffer[x] != '\r' && buffer[x] != '\n' && x < width) {
            char ch = buffer[x];
            if (ch == 'P') {
                game->hero.x = x;
                game->hero.y = y;
                ch = '.';
            } else if (ch == 'M') {
                // Проверяем, нет ли уже монстра на этих координатах
                bool monster_already_exists = false;
                for (int m_check = 0; m_check < game->monster_count; m_check++) {
                    if (game->monsters[m_check].x == x && game->monsters[m_check].y == y) {
                        monster_already_exists = true;
                        break;
                    }
                }

                // Добавляем монстра только если его там еще нет
                if (!monster_already_exists) {
                    add_monster(game, x, y, 25, 8, 'M');
                }

                ch = '.';
            }
            set_cell(&game->map, x, y, ch);
            x++;
        }
        while (x < width) {
            set_cell(&game->map, x, y, ' ');
            x++;
        }
        y++;
    }

    fclose(file);
    return OK;
}

// Полное сохранение игры (герой, инвентарь, баффы, карта)
bool save_game(const char *filename, const GameState *game) {
    if (filename == NULL || game == NULL) return false;

    FILE *file = fopen(filename, "w");
    if (file == NULL) return false;

    fprintf(file, "HERO %d %d %d %d %u %u %u %u\n",
            game->hero.x, game->hero.y, game->hero.hp, game->hero.max_hp,
            game->hero.damage, game->hero.gold, game->hero.depth, game->hero.status);

    fprintf(file, "INVENTORY %d\n", game->hero.inventory_count);
    for (int i = 0; i < game->hero.inventory_count; i++) {
        fprintf(file, "%s|%d|%u|%d\n",
                game->hero.inventory[i].name,
                game->hero.inventory[i].stat_bonus,
                game->hero.inventory[i].effect_flag,
                game->hero.inventory[i].duration);
    }

    fprintf(file, "BUFFS %d\n", game->hero.buff_count);
    for (int i = 0; i < game->hero.buff_count; i++) {
        fprintf(file, "%s|%d|%d|%u\n",
                game->hero.buffs[i].name,
                game->hero.buffs[i].turns_left,
                game->hero.buffs[i].stat_bonus,
                game->hero.buffs[i].effect_flag);
    }

    fprintf(file, "MAP %d %d\n", game->map.width, game->map.height);
    for (int y = 0; y < game->map.height; y++) {
        for (int x = 0; x < game->map.width; x++) {
            fputc(get_cell(&game->map, x, y), file);
        }
        fputc('\n', file);
    }

    fclose(file);
    return true;
}

// Полная загрузка игры с обработкой ошибок
GameError load_game(const char *filename, GameState *game) {
    if (filename == NULL || game == NULL) return ERR_FILE_NOT_FOUND;

    FILE *file = fopen(filename, "r");
    if (file == NULL) return ERR_FILE_NOT_FOUND;

    char buffer[512];

    if (fgets(buffer, sizeof(buffer), file) == NULL) { fclose(file); return ERR_CORRUPTED_FORMAT; }
    if (sscanf(buffer, "HERO %d %d %d %d %u %u %u %u",
               &game->hero.x, &game->hero.y, &game->hero.hp, &game->hero.max_hp,
               &game->hero.damage, &game->hero.gold, &game->hero.depth, &game->hero.status) != 8) {
        fclose(file); return ERR_CORRUPTED_FORMAT;
    }

    if (fgets(buffer, sizeof(buffer), file) == NULL) { fclose(file); return ERR_CORRUPTED_FORMAT; }
    if (sscanf(buffer, "INVENTORY %d", &game->hero.inventory_count) != 1) { fclose(file); return ERR_CORRUPTED_FORMAT; }
    for (int i = 0; i < game->hero.inventory_count; i++) {
        if (fgets(buffer, sizeof(buffer), file) == NULL) { fclose(file); return ERR_CORRUPTED_FORMAT; }
        char name[32];
        int stat_bonus;
        unsigned int effect_flag;
        int duration;
        if (sscanf(buffer, "%31[^|]|%d|%u|%d", name, &stat_bonus, &effect_flag, &duration) == 4) {
            strcpy(game->hero.inventory[i].name, name);
            game->hero.inventory[i].stat_bonus = stat_bonus;
            game->hero.inventory[i].effect_flag = effect_flag;
            game->hero.inventory[i].duration = duration;
        }
    }

    if (fgets(buffer, sizeof(buffer), file) == NULL) { fclose(file); return ERR_CORRUPTED_FORMAT; }
    if (sscanf(buffer, "BUFFS %d", &game->hero.buff_count) != 1) { fclose(file); return ERR_CORRUPTED_FORMAT; }
    for (int i = 0; i < game->hero.buff_count; i++) {
        if (fgets(buffer, sizeof(buffer), file) == NULL) { fclose(file); return ERR_CORRUPTED_FORMAT; }
        char name[32];
        int turns_left;
        int stat_bonus;
        unsigned int effect_flag;
        if (sscanf(buffer, "%31[^|]|%d|%d|%u", name, &turns_left, &stat_bonus, &effect_flag) == 4) {
            strcpy(game->hero.buffs[i].name, name);
            game->hero.buffs[i].turns_left = turns_left;
            game->hero.buffs[i].stat_bonus = stat_bonus;
            game->hero.buffs[i].effect_flag = effect_flag;
        }
    }

    if (fgets(buffer, sizeof(buffer), file) == NULL) { fclose(file); return ERR_CORRUPTED_FORMAT; }
    int width, height;
    if (sscanf(buffer, "MAP %d %d", &width, &height) != 2) { fclose(file); return ERR_CORRUPTED_FORMAT; }

    if (game->map.cells != NULL) {
        free(game->map.cells);
    }

    game->map.width = width;
    game->map.height = height;
    game->map.cells = (char *)malloc((size_t)width * (size_t)height * sizeof(char));
    if (game->map.cells == NULL) { fclose(file); return ERR_OUT_OF_MEMORY; }

    for (int y = 0; y < height; y++) {
        if (fgets(buffer, sizeof(buffer), file) == NULL) { fclose(file); return ERR_CORRUPTED_FORMAT; }
        for (int x = 0; x < width; x++) {
            set_cell(&game->map, x, y, buffer[x]);
        }
    }

    fclose(file);
    return OK;
}

// Процедурная генерация с сохранением в файл и последующей выгрузкой
GameError random_map(GameState *game, int width, int height) {
    if (width < 10 || height < 8) return ERR_INVALID_SIZE;

    game->map.width = width;
    game->map.height = height;
    game->map.cells = (char *)malloc((size_t)width * (size_t)height * sizeof(char));
    if (game->map.cells == NULL) return ERR_OUT_OF_MEMORY;

    int attempts = 0;
    while (attempts < 100) {
        if (game->monsters != NULL) {
            free(game->monsters);
            game->monsters = NULL;
        }
        game->monster_count = 0;
        game->monster_capacity = 0;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                    set_cell(&game->map, x, y, '#');
                } else {
                    if (rand() % 100 < 20) {
                        set_cell(&game->map, x, y, '#');
                    } else {
                        set_cell(&game->map, x, y, '.');
                    }
                }
            }
        }

        int hero_spawned = 0;
        while (!hero_spawned) {
            int rx = 1 + rand() % (width - 2);
            int ry = 1 + rand() % (height - 2);

            // Убедимся, что герой не спавнится на выходе
            if (!(rx == width - 1 && ry == height / 2)) {
                game->hero.x = rx;
                game->hero.y = ry;
                set_cell(&game->map, game->hero.x, game->hero.y, 'P');
                hero_spawned = 1;
            }
        }

        int exit_x = width - 1;
        int exit_y = height / 2;
        set_cell(&game->map, exit_x, exit_y, 'E');
        set_cell(&game->map, exit_x - 1, exit_y, '.'); // Гарантируем свободный проход к выходу

        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                if ((x == 1 && y == 1) || (x == exit_x && y == exit_y) || (x == exit_x - 1 && y == exit_y)) continue;
                int rnd = rand() % 100;
                if (rnd < 3) {
                    set_cell(&game->map, x, y, '$'); // Золото (3%)
                } else if (rnd < 5) {
                    set_cell(&game->map, x, y, 'B'); // Зелье (2%)
                } else if (rnd < 4) {
                    set_cell(&game->map, x, y, 'P'); // Зелье (2%)
                } else if (rnd < 6) {
                    set_cell(&game->map, x, y, 'I'); // Предмет I (1%)
                }
            }
        }

        int spawned_monsters = 0;
        int safe_guard = 0;
        while (spawned_monsters < 3 && safe_guard < 200) {
            int rx = 1 + rand() % (width - 2);
            int ry = 1 + rand() % (height - 2);

            if (get_cell(&game->map, rx, ry) == '.' &&
                !(rx == game->hero.x && ry == game->hero.y) &&
                !(rx == exit_x && ry == exit_y)) {

                add_monster(game, rx, ry, 25, 8, 'M');
                spawned_monsters++;
            }
            bool cell_occupied = false;
            for (int m = 0; m < game->monster_count; m++) {
                if (game->monsters[m].x == rx && game->monsters[m].y == ry) {
                    cell_occupied = true;
                    break;
                }
            }

            if (!cell_occupied && get_cell(&game->map, rx, ry) == '.' &&
                !(rx == game->hero.x && ry == game->hero.y) &&
                !(rx == exit_x && ry == exit_y)) {

                add_monster(game, rx, ry, 25, 8, 'M');
                spawned_monsters++;
            }
            safe_guard++;
        }

        if (check_path_exists(&game->map, game->hero.y, game->hero.x)) {
            break;
        }

        attempts++;
    }

    if (attempts >= 100) {
        if (game->monsters != NULL) {
            free(game->monsters);
            game->monsters = NULL;
        }
        free(game->map.cells);
        game->map.cells = NULL;
        return ERR_CORRUPTED_FORMAT;
    }

    // 7. Сохраняем карту в файл level.txt
    if (!save_map("level.txt", game)) {
        if (game->monsters != NULL) {
            free(game->monsters);
            game->monsters = NULL;
        }
        free(game->map.cells);
        game->map.cells = NULL;
        return ERR_CORRUPTED_FORMAT;
    }

    free(game->map.cells);
    game->map.cells = NULL;

    return load_map("level.txt", game);
}

void p_map(const GameState *game) {
    if (game == NULL || game->map.cells == NULL) return;

    for (int y = 0; y < game->map.height; y++) {
        for (int x = 0; x < game->map.width; x++) {
            if (y == game->hero.y && x == game->hero.x) {
                putchar('P');
            } else {
                bool printed_monster = false;
                for (int m = 0; m < game->monster_count; m++) {
                    if (game->monsters[m].x == x && game->monsters[m].y == y && game->monsters[m].hp > 0) {
                        putchar(game->monsters[m].type_symbol);
                        printed_monster = true;
                        break;
                    }
                }
                if (!printed_monster) {
                    putchar(get_cell(&game->map, x, y));
                }
            }
        }
        putchar('\n');
    }
}

bool addr(const Map *map, int *p_x, int *p_y, const char *validcmd) {
    if (p_x == NULL || p_y == NULL || validcmd == NULL || map == NULL) return false;
    int next_x = (*p_x);
    int next_y = (*p_y);

    if (strcmp(validcmd, "south") == 0) { next_y = next_y + 1; }
    if (strcmp(validcmd, "north") == 0) { next_y = next_y - 1; }
    if (strcmp(validcmd, "east") == 0)  { next_x = next_x + 1; }
    if (strcmp(validcmd, "west") == 0)  { --next_x; }

    char cell_type = get_cell(map, next_x, next_y);

    if (next_x < 0 || next_x >= map->width || next_y < 0 || next_y >= map->height || cell_type == '#') {
        if (next_x < 0 || next_x >= map->width || next_y < 0 || next_y >= map->height) {
            printf("Нельзя выходить за карту\n");
        } else {
            printf("Вы уперлись в стену\n");
            --m_count;
        }
        return false;
    }

    (*p_x) = next_x;
    (*p_y) = next_y;
    return true;
}

void p_check(const GameState *game) {
    if (game == NULL || game->map.cells == NULL) return;
    if (check_path_exists(&game->map, game->hero.y, game->hero.x)) {
        printf("Выход есть\n\n");
    } else {
        printf("Выхода нет\n\n");
    }
}

bool c_map(const char *validcmd, GameState *game) {
    if (game == NULL || validcmd == NULL) return false;

    int prev_x = game->hero.x;
    int prev_y = game->hero.y;

    if (!addr(&game->map, &game->hero.x, &game->hero.y, validcmd)) {
        return true;
    }

    // Проверяем столкновение с монстром на новой клетке
    for (int m = 0; m < game->monster_count; m++) {
        if (game->monsters[m].x == game->hero.x && game->monsters[m].y == game->hero.y && game->monsters[m].hp > 0) {

            int target_x = game->hero.x;
            int target_y = game->hero.y;

            game->hero.x = prev_x;
            game->hero.y = prev_y;

            handle_monster_combat(game, m);

            if (m >= game->monster_count || game->monsters[m].hp <= 0) {
                game->hero.x = target_x;
                game->hero.y = target_y;
            }

            if (game->hero.hp <= 0) {
                return false; // Игра окончена (герой умер)
            }
            return true; // Ход завершен ударом в бою
        }
    }

    char place = get_cell(&game->map, game->hero.x, game->hero.y);

    if (place == '$') {
        game->hero.gold = game->hero.gold + 1;
        set_cell(&game->map, game->hero.x, game->hero.y, '.');
        printf("Вы подобрали золото! (+1 золото)\n");
    }
    else if (place == 'B') {
        set_cell(&game->map, game->hero.x, game->hero.y, '.');
        if (game->hero.inventory_count < 10) {
            Item item;
            int potion_type = rand() % 2;

            if (potion_type == 0) {
                strcpy(item.name, "Зелье лечения");
                item.stat_bonus = 20;
                item.effect_flag = 0;
                item.duration = 0;
            } else {
                strcpy(item.name, "Зелье силы");
                item.stat_bonus = 10;
                item.duration = 5;
                item.effect_flag = 0;
            }

            game->hero.inventory[game->hero.inventory_count++] = item;
            printf("Предмет [%s] добавлен в инвентарь!\n", item.name);
        } else {
            printf("Инвентарь полон! Нельзя подобрать предмет.\n");
        }
    }
    else if (place == 'I') {
        set_cell(&game->map, game->hero.x, game->hero.y, '.');
        if (game->hero.inventory_count < 10) {
            Item item;
            strcpy(item.name, "Щит Теней");
            item.stat_bonus = 0;
            item.effect_flag = invisible;
            item.duration = 3;
            game->hero.inventory[game->hero.inventory_count++] = item;
            printf("Предмет [Щит Теней] добавлен в инвентарь!\n");
        }
    }

    if (place == 'E') {
        game->hero.depth++;
        printf("Вы спустились на следующий уровень %u!\n\n", game->hero.depth);

        free(game->map.cells);
        GameError err = random_map(game, 16, 8);
        if (err != OK) {
            printf("Ошибка генерации нового уровня!\n");
            return false;
        }
        printf("Новый этаж успешно сгенерирован:\n");
        p_map(game);
    }

    return true;
}