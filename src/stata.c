#include <stdio.h>
#include "stata.h"

void update_buffs(Hero *hero) {
    if (hero == NULL) return;

    for (int i = 0; i < hero->buff_count; i++) {
        hero->buffs[i].turns_left--;
        printf("Эффект '%s': осталось ходов: %d\n", hero->buffs[i].name, hero->buffs[i].turns_left);

        if (hero->buffs[i].turns_left <= 0) {
            printf("Эффект '%s' завершил свое действие!\n", hero->buffs[i].name);
            if (hero->buffs[i].stat_bonus != 0) {
                hero->damage -= hero->buffs[i].stat_bonus;
            }
            if (hero->buffs[i].effect_flag != 0) {
                hero->status &= ~hero->buffs[i].effect_flag;
            }

            for (int j = i; j < hero->buff_count - 1; j++) {
                hero->buffs[j] = hero->buffs[j + 1];
            }
            hero->buff_count--;
            i--;
        }
    }
}

struct {
    unsigned int maska;
    const char * ef;
}eflist[] = {
    {invisible, "invisible"},
    {poisoned, "poisoned"},
    {blinded, "blinded"},
    {frozen, "frozen"},
    {burned, "burned"},
    {sleeping, "sleeping"},
};
const int numef = sizeof(eflist) / sizeof(eflist[0]);
const char *eflst =
        "Можно применить данные эффекты или снять их через команды set[name] и clear[name]:  \n"
        "invinsible     | невидимость       \n"
        "poisoned       | отравление      \n"
        "blinded        | ослепление  \n"
        "frozen         | окаменение   \n"
        "burned         | горение  \n"
        "sleeping       | сон \n"
;

void p_status(const Hero *hero) {
    int hp_prcnt = (int)(((float)(*hero).hp / (float)(*hero).max_hp) * 100.0f);

    printf("\n--------- Герой ---------\n");
    printf("Здоровье: %d/%d (%d%%)\n", (*hero).hp, (*hero).max_hp, hp_prcnt);
    printf("Урон: %d\n", (*hero).damage);
    printf("Золото: %u\n", (*hero).gold);
    printf("Уровень: %u\n", (*hero).depth);
    printf("Состояние:  \n");

    printf("Инвентарь (%d/10):\n", hero->inventory_count);
    if (hero->inventory_count == 0) {
        printf("  (пусто)\n");
    } else {
        for (int i = 0; i < hero->inventory_count; i++) {
            printf("  [%d] %s\n", i + 1, hero->inventory[i].name);
        }
    }

    printf("Активных баффов: %d\n", (*hero).buff_count);
    printf("Состояние:  \n");

    if ((*hero).status == 0) {
        printf("На вас не наложены никакие эффекты\n");
    } else {
        if (((*hero).status & invisible) != 0) printf("- Невидимость\n");
        if (((*hero).status & poisoned) != 0) printf("- Отравление\n");
        if (((*hero).status & blinded) != 0) printf("- Ослепление\n");
        if (((*hero).status & frozen) != 0) printf("- Окаменение\n");
        if (((*hero).status & burned) != 0) printf("- Горение\n");
        if (((*hero).status & sleeping) != 0) printf("- Сон\n");
    }
    printf("\n-------------------------\n\n");
}
