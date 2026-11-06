cmake_minimum_required(VERSION 3.20)
project(DungeonC__)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Указываем компилятору искать .h файлы в папке header
include_directories(header)

add_executable(DungeonC__
        src/main.cpp
        src/map.cpp
        src/hero.cpp
        header/map.h
        header/hero.h
        src/run.cpp
        header/run.h
        header/run.h
)