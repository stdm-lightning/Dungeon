#include <stdio.h>
#include <string.h>
#include <stdbool.h>
int main(void) {
    printf("Welcome to the dungeon!\n");
    printf("You can use these commands: look, north, south, east, west, quit\n\n");
    const char* map[] ={
        "######",
        "#___#",
        "#___#",
        "#___#",
        "#####"
    };
    bool is_running = true;
    char command[15];
    scanf("%s15\n", command);
    printf("%s\n", command);
    size_t size = sizeof(map)/sizeof(const char*);
        for (size_t i=0; i <size; ++i){
        printf("%s\n", map[i]);
    }
    while (is_running) {
        if (strcmp(command, "quit") == 0) {
            printf("You left game");
            is_running = false;
        }
    }
    return 0;
}

