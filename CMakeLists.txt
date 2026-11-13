cmake_minimum_required(VERSION 4.3)
project(dynamic_massiv_project C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(dynamic_massiv_project
        dynamic_array.c
        dynamic_array.h
        main.c
        )
