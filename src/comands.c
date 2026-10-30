#include "comands.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "map.h"
#include "stata.h"
#include "damage.h"
#include "effects.h"

extern const char *validcmd;
extern const char *eflst;
extern int m_count;

void low(char *str) {
    if (str == NULL) return;
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = str[i] + ('a' - 'A');
        }
        i = i + 1;
    }
}

bool manual(const char *str, int *out_value) {
    if (str == NULL || out_value == NULL || str[0] == '\0') {
        printf("Ошибка: Аргумент команды отсутствует!\n");
        return false;
    }

    int i = 0;
    long long result = 0;

    while (str[i] != '\0') {
        if (str[i] < '0' || str[i] > '9') {
            printf("Ошибка: Аргумент содержит нецифровые символы '%c'!\n", str[i]);
            return false;
        }

        int digit = str[i] - '0';
        result = result * 10 + digit;

        if (result > 2147483647) {
            printf("Ошибка: Число слишком велико!\n");
            return false;
        }
        i = i + 1;
    }

    (*out_value) = (int)result;
    return true;
}

bool check_command(char *line, GameState *game) {
    if (line == NULL || game == NULL) return false;
    low(line);

    char *words[10];
    int w_count = 0;
    int i = 0;
    bool in_word = false;
    while (line[i] != '\0') {
        if (line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\n') {
            line[i] = '\0';
            in_word = false;
        } else {
            if (!in_word) {
                if (w_count < 10) {
                    words[w_count] = &(line[i]);
                    w_count = w_count + 1;
                }
                in_word = true;
            }
        }
        i = i + 1;
    }
    if (w_count == 0) return true;
    char *command = words[0];

    if (strcmp(command, "quit") == 0) {
        printf("Вы покинули игру\n");
        return false;
    }
    if (strcmp(command, "check") == 0) {
        printf("Проверка выхода:\n");
        p_check(game);
    }
    
    char *dir = NULL;
    if (strcmp(command, "go") == 0 && w_count > 1) {
        dir = words[1];
    } else {
        dir = command;
    }

    if (strcmp(dir, "north") == 0 || strcmp(dir, "n") == 0) {
        printf("Вы идете на север\n");
        if (!c_map("north", game)) return false;
        ++m_count;
    }
    else if (strcmp(dir, "south") == 0 || strcmp(dir, "s") == 0) {
        printf("Вы идете на юг\n");
        if (!c_map("south", game)) return false;
        ++m_count;
    }
    else if (strcmp(dir, "east") == 0 || strcmp(dir, "e") == 0) {
        printf("Вы идете на восток\n");
        if (!c_map("east", game)) return false;
        ++m_count;
    }
    else if (strcmp(dir, "west") == 0 || strcmp(dir, "w") == 0) {
        printf("Вы идете на запад\n");
        if (!c_map("west", game)) return false;
        ++m_count;
    }
    else if (strcmp(command, "look") == 0 || strcmp(command, "l") == 0) {
        printf("Карта подземелья\n");
        p_map(game);
    }
    else if (strcmp(command, "set") == 0) {
        if (w_count > 1) {
            if (!sete(&game->hero, words[1])) {
                printf("Неизвестный эффект!\n\n");
            }
        } else {
            printf("Укажите имя эффекта!\n\n");
        }
    }
    else if (strcmp(command, "clear") == 0) {
        if (w_count > 1) {
            if (!dele(&game->hero, words[1])) printf("Неизвестный эффект!\n\n");
        } else {
            printf("Укажите имя эффекта!\n\n");
        }
    }
    else if (strcmp(command, "hit") == 0) {
        int damage_taken = 0;
        if (w_count > 1 && manual(words[1], &damage_taken)) {
            if (!t_damage(&(game->hero.hp), damage_taken)) return false;
        } else {
            printf("Используйте: hit N \n\n");
        }
    }
    else if (strcmp(command, "use") == 0) {
        int item_id = 0;
        if (w_count > 1 && manual(words[1], &item_id)) {
            int index = item_id - 1;
            if (index >= 0 && index < game->hero.inventory_count) {
                Item item = game->hero.inventory[index];
                printf("Вы использовали предмет: %s\n", item.name);

                // Если это мгновенное зелье лечения
                if (strcmp(item.name, "Зелье лечения") == 0) {
                    game->hero.hp += item.stat_bonus; // например, +20 HP
                    if (game->hero.hp > game->hero.max_hp) {
                        game->hero.hp = game->hero.max_hp;
                    }
                    printf("Восстановлено %d HP! Текущее HP: %d/%d\n", item.stat_bonus, game->hero.hp, game->hero.max_hp);
                }
                else {
                    // Для всех временных баффов (Зелье силы, Щит и т.д.)
                    if (game->hero.buff_count < 5) {
                        strcpy(game->hero.buffs[game->hero.buff_count].name, item.name);
                        game->hero.buffs[game->hero.buff_count].turns_left = item.duration; // Количество ходов действия
                        game->hero.buffs[game->hero.buff_count].stat_bonus = item.stat_bonus; // Прибавка к урону/статам
                        game->hero.buffs[game->hero.buff_count].effect_flag = item.effect_flag;

                        // Сразу применяем бонус (например, к урону)
                        game->hero.damage += item.stat_bonus;
                        if (item.effect_flag != 0) {
                            game->hero.status |= item.effect_flag;
                        }
                        game->hero.buff_count++;
                        printf("Эффект '%s' активирован на %d ходов!\n", item.name, item.duration);
                    } else {
                        printf("Слишком много активных эффектов одновременно!\n");
                    }
                }

                // Удаляем использованный предмет из инвентаря и сдвигаем массив
                for (int j = index; j < game->hero.inventory_count - 1; j++) {
                    game->hero.inventory[j] = game->hero.inventory[j + 1];
                }
                game->hero.inventory_count--;
            } else {
                printf("Неверный номер предмета в инвентаре!\n");
            }
        } else {
            printf("Использование: use [номер предмета]\n");
        }
    }
    else if (strcmp(command, "save") == 0) {
        char *filename = "savegame.txt";
        if (w_count > 1) filename = words[1];
        if (save_game(filename, game)) {
            printf("Игра успешно сохранена в '%s'\n\n", filename);
        } else {
            printf("Ошибка сохранения игры!\n\n");
        }
    }
    else if (strcmp(command, "load") == 0) {
        char *filename = "savegame.txt";
        if (w_count > 1) filename = words[1];
        GameError err = load_game(filename, game);
        if (err == OK) {
            printf("Игра успешно загружена из '%s'!\n\n", filename);
            p_map(game);
        } else {
            printf("Ошибка загрузки (код: %d)\n\n", err);
        }
    }
    else if (strcmp(command, "restart") == 0) {
        printf("Начинаем новую игру...\n");
        game->hero.hp = 100;
        game->hero.max_hp = 100;
        game->hero.damage = 15;
        game->hero.gold = 0;
        game->hero.depth = 1;
        game->hero.status = 0;
        game->hero.inventory_count = 0;
        game->hero.buff_count = 0;
        m_count = 1;

        if (game->map.cells != NULL) {
            free(game->map.cells);
            game->map.cells = NULL;
        }

        if (random_map(game, 16, 8) != OK) {
            printf("Ошибка генерации нового уровня!\n");
            return false;
        }
        printf("Новая игра начата!\n\n");
        p_map(game);
    }
    else if (strcmp(command, "status") == 0) {
         p_status(&game->hero);
    }
    else if (strcmp(command, "effects") == 0) {
        printf("%s\n", eflst);
    }
    else if (strcmp(command, "help") == 0) {
        printf("%s\n", validcmd);
    }
    else {
        printf("Неверная команда\n%s", validcmd);
    }
    return true;
}