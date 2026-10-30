#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "run.h"
#include "map.h"
#include "comands.h"
#include "stata.h"
#include <stdlib.h>
#include <time.h>

int m_count = 1;
const char *validcmd =
        "Обозначения на карте: P: Герой | M: Монстр | #: Стена | .: Проход | $: Золото | B: Зелье | I: Щит | E: Выход\n"
        "Ты можешь использовать эти команды: \n"
        "look           | Посмотреть карту        \n"
        "check          | Проверить доступен ли выход из подземелья\n"
        "north / south  | Идти на север / юг      \n"
        "east / west    | Идти на восток / запад  \n"
        "status         | Характеристики и инвентарь\n"
        "effects        | Эффекты \n"
        "set [name]     | Включить бит состояния  \n"
        "clear [name]   | Выключить бит состояния \n"
        "hit [N]        | Нанести герою N урона   \n"
        "use [N]        | Использовать предмет N  \n"
        "save [file]    | Сохранить игру в файл   \n"
        "load [file]    | Загрузить игру из файла \n"
        "restart        | Начать новую игру с нуля\n"
        "help           | Показать меню команд       \n"
        "quit           | Выйти из игры           \n"
;

void run_game() {
    srand((unsigned int)time(NULL));
    printf("ДОБРО ПОЖАЛОВАТЬ В ПОДЗЕМЕЛЬЕ!\n\n");
    GameState game;
    game.hero.hp = 100;
    game.hero.max_hp = 100;
    game.hero.damage = 15;
    game.hero.gold = 0;
    game.hero.depth = 1;
    game.hero.status = 0;
    game.hero.inventory_count = 0;
    game.hero.buff_count = 0;
    game.map.cells = NULL;

    // Генерация стартового уровня
    if (random_map(&game, 16, 8) != OK) {
        printf("Ошибка генерации стартового уровня!\n");
        return;
    }

    printf("%s", validcmd);
    printf("\n\n");

    printf("Карта подземелья (Этаж 1)\n");
    p_map(&game);

    bool is_running = true;
    char input_buffer[64];
    while (is_running) {
        printf("Turn: %d\n" , m_count);
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            break;
        }
        int len = 0;
        while (input_buffer[len] != '\0') {
            len = len + 1;
        }
        if (len > 0 && input_buffer[len - 1] == '\n') {
            input_buffer[len - 1] = '\0';
        }

        is_running = check_command(input_buffer, &game);
        if (!is_running) {
            break;
        }

        update_buffs(&game.hero);
    }

    // Автосохранение при выходе
    if (save_game("autosave.txt", &game)) {
        printf("Выполнено автосохранение игры в 'autosave.txt'.\n");
    }

    free_game_state(&game);
}