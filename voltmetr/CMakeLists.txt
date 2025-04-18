# Минимальная версия CMake
cmake_minimum_required(VERSION 3.10)

# Название проекта
project(voltmetr)

# Найдите пути к NI-VISA
find_path(VISA_INCLUDE_DIR NAMES visa.h
          PATHS /usr/include/ni-visa  # Обновлённый путь к заголовочным файлам
                ENV CPATH)

find_library(VISA_LIBRARY NAMES visa
             PATHS /usr/lib           # Путь к библиотекам (обычно в /usr/lib или /usr/lib64)
                   /usr/lib64
                   ENV LD_LIBRARY_PATH)

# Проверка, найдены ли NI-VISA заголовочные файлы и библиотеки
if(NOT VISA_INCLUDE_DIR OR NOT VISA_LIBRARY)
    message(FATAL_ERROR "NI-VISA не найден. Убедитесь, что NI-VISA установлен.")
endif()

# Добавьте исполняемый файл
add_executable(voltmetr main.cpp myvisa.cpp mm.cpp)

# Подключите NI-VISA к проекту
target_include_directories(voltmetr PRIVATE ${VISA_INCLUDE_DIR})
target_link_libraries(voltmetr PRIVATE ${VISA_LIBRARY} dl)
