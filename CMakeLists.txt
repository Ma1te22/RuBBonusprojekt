cmake_minimum_required(VERSION 4.2)
project(RuBBonusprojekt C)

set(CMAKE_C_STANDARD 11)

add_executable(RuBBonusprojekt writer.c)

target_link_libraries(RuBBonusprojekt PRIVATE statgrab)
