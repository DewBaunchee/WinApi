cmake_minimum_required(VERSION 3.19)
project(Shared C)

set(CMAKE_C_STANDARD 99)

add_library(Shared SHARED library.c library.h)