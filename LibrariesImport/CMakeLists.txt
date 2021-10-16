cmake_minimum_required(VERSION 3.19)
project(LibrariesImport C)

set(CMAKE_C_STANDARD 99)

add_executable(LibrariesImport main.c)
add_library(libStatic.a main.c)
target_link_libraries(LibrariesImport libStatic.a)