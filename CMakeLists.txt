cmake_minimum_required(VERSION 3.22)
project(APIProject_2022 C)

set(CMAKE_C_STANDARD 11)
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -static-libasan")

add_executable(APIProject_2022 main.c)