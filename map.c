#include "map.h"
#include <stdio.h>

void p_map() {
    const char* map[]={
        "######",
        "#___#",
        "#___#",
        "#___#",
        "#####"
    };
    int strok = sizeof(map) / sizeof(map[0]);
    for (int i = 0; i < strok; i++) {
        printf("%s\n", map[i]);
    }
}
