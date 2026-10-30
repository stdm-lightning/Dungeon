#include <stdio.h>
#include <string.h>
#include "damage.h"
#include "monster.h"
#include "run.h"
void handle_monster_combat(GameState *game, int monster_index) {
    if (game == NULL || monster_index < 0 || monster_index >= game->monster_count) return;

    Monster *m = &game->monsters[monster_index];

    printf("\n--- БОЙ С МОНСТРОМ (%c) ---\n", m->type_symbol);

    // Героя бьет монстр
    game->hero.hp -= m->damage;
    printf("Монстр атакует вас и наносит %d урона! У вас осталось HP: %d/%d\n",
           m->damage, game->hero.hp, game->hero.max_hp);

    // Герой бьет монстра в ответ
    m->hp -= game->hero.damage;

    // Защита от отрицательного здоровья для красивого вывода
    int display_monster_hp = m->hp;
    if (display_monster_hp < 0) {
        display_monster_hp = 0;
    }

    printf("Вы контратакуете и наносите монстру %d урона! У монстра осталось HP: %d\n",
           game->hero.damage, display_monster_hp);

    // Проверяем смерть монстра
    if (m->hp <= 0) {
        printf("Монстр повержен!\n");
        remove_monster(game, monster_index); // Удаляем из динамического массива
    }

    // Проверяем смерть героя
    if (game->hero.hp <= 0) {
        game->hero.hp = 0;
        printf("Вы погибли в бою!\n");
    }
    printf("---------------------------\n\n");
}

bool t_damage(int *p_hp, int damage) {

    if (p_hp == NULL) {
        return false;
    }

    if (damage <= 0) {
        printf("Неверный урон\n");
        return true;
    }
    (*p_hp) = (*p_hp) - damage;
    printf("Герою нанесено %d урона. Текущее HP: %d\n\n", damage, (*p_hp));
    if ((*p_hp) <= 0) {
        (*p_hp) = 0;
        printf("Вы мертвы\n");
        return false;
    }
    printf("\n");

    return true; //

}
