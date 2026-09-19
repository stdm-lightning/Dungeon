#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "run.h"
#include "map.h"
void run_game() {
    printf("Welcome to the dungeon!\n");
    const char * validcmd = "You can use these commands: look, north, south, east, west, quit\n";
    int m_count = 1;
    bool is_running = true;
    char command[15];
    p_map();

    while (is_running) {
        printf("\n");
        printf("Turn: %d\n" , m_count);
        scanf("%15s", command);
        if (strcmp(command, "quit") == 0) {
            printf("You left game");
            is_running = false;
        }
        else if (strcmp(command, "north") == 0) {
            printf("You are going north\n");
            m_count++;
        }
        else if (strcmp(command, "south") == 0) {
            printf("You are going south\n");
            m_count++;
        }
        else if (strcmp(command, "east") == 0) {
            printf("You are going east\n");
            m_count++;
        }
        else if (strcmp(command, "west") == 0) {
            printf("You are going west\n");
            m_count++;
        }
        else if (strcmp(command, "look") == 0) {
            printf("You are looking for\n");
            m_count++;
        }
        else {
            printf("Invalid command\n");
            printf("%s", validcmd);
        }
    }

}