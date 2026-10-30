cmake_minimum_required(VERSION 3.10)
project(Dungeon)

set(CMAKE_C_STANDARD 99)
include_directories(include)
add_executable(
        Dungeon
        src/main.c
        src/map.c
        include/map.h
        include/run.h
        src/run.c
        include/comands.h
        src/comands.c
        src/stata.c
        include/stata.h
        src/damage.c
        include/damage.h
        include/comands.h
        src/effects.c
        include/monster.h
        src/monster.c
)
