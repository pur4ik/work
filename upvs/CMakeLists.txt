# Указываем минимальную версию CMake
cmake_minimum_required(VERSION 3.10)

# Устанавливаем имя проекта
project(UPVS)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Добавляем исходные файлы в проект
set(SOURCES
    src/main.cpp
    src/data.cpp
    src/netcat.cpp
    src/upvs.cpp
    src/utils.cpp
)

# Определяем пути
set(XML_SOURCE_DIR "${CMAKE_SOURCE_DIR}/xml")
set(XML_DEST_DIR "${CMAKE_BINARY_DIR}")

# Список XML-файлов для копирования
set(XML_FILES
    request.xml
    response.xml
)

# Создаем кастомный таргет для копирования файлов
add_custom_target(CopyXMLFiles ALL
    COMMENT "Copying XML files to build directory"
)

# Добавляем команды копирования для каждого файла
foreach(xml_file IN LISTS XML_FILES)
    add_custom_command(
        TARGET CopyXMLFiles POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${XML_SOURCE_DIR}/${xml_file}"
            "${XML_DEST_DIR}/${xml_file}"
        COMMENT "Copying ${xml_file} to build directory"
    )
endforeach()

# Создаем исполняемый файл
add_executable(upvs ${SOURCES})

# Включаем заголовочные файлы
include_directories(${CMAKE_SOURCE_DIR})