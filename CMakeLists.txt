cmake_minimum_required(VERSION 3.15)

project(neoshell LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(neoshell
    src/main.cpp
)

target_compile_options(neoshell PRIVATE -Wall -Wextra)
