# Минимальная версия CMake
cmake_minimum_required(VERSION 3.10)

# Название проекта
project(bp)

# Укажите язык проекта
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Добавьте исходные файлы
add_executable(bp main.cpp uart.cpp utils.cpp bp.cpp)

# Укажите пути к заголовочным файлам NI-VISA
