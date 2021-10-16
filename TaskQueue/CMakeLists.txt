cmake_minimum_required(VERSION 3.19)
project(TaskQueue C)

set(CMAKE_C_STANDARD 99)

add_executable(TaskQueue main.c queue/task_queue.c merge_sort/merge_sort.c util/sort_util.c stopwatch/stopwatch.c)