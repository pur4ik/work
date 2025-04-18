cmake_minimum_required(VERSION 3.10)
project(parse)

set(CMAKE_CXX_STANDARD 17)

# Добавляем исходные файлы
add_executable(parse main.cpp buev.cpp)
