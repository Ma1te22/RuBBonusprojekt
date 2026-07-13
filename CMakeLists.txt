cmake_minimum_required(VERSION 3.28.3)
project(RuBBonusprojekt C)

set(CMAKE_C_STANDARD 11)

add_executable(RuBBonusprojekt
        writer.c
        reader.c
        main.c)

target_link_libraries(RuBBonusprojekt PRIVATE statgrab)
