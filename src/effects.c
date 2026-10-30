#include <stdio.h>
#include <string.h>
#include "effects.h"
extern const struct {unsigned int maska; const char *ef; } eflist[];
extern const int numef;

bool sete(Hero *hero, const char *ename) {
    for (int i = 0; i < numef; i++) {
        if (strcmp(ename, eflist[i].ef) == 0) {
            (*hero).status |= eflist[i].maska;
            printf("Наложен эффект '%s' \n\n", eflist[i].ef);
            return true;
        }
    }
    return false;
}

bool dele(Hero *hero, const char *ename) {
    for (int i = 0; i < numef; i++) {
        if (strcmp(ename, eflist[i].ef) == 0) {
            (*hero).status &= ~eflist[i].maska;
            printf("Эффект '%s' снят \n\n", eflist[i].ef);
            return true;
        }
    }
    return false;
}

