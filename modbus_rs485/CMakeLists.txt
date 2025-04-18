cmake_minimum_required(VERSION 3.16)

project(modbus_rs485)

set(CMAKE_CXX_STANDARD 17)

# Исходные файлы проекта
set(SOURCES
    src/main.cpp
    src/freq/modbus.cpp  
    src/utils.cpp
    src/freq/freq_converter.cpp
    src/load/socket.cpp
    src/bp/bp.cpp
    src/bp/uart.cpp
    src/buev/buev_tcp.cpp
    src/buev/buev_udp.cpp
    src/buev/data.cpp
    src/buev/led.cpp
    src/buev/triggers.cpp
    src/mm/mm.cpp
    src/mm/myvisa.cpp
    src/upvs/netcat.cpp
    src/upvs/upvs.cpp
    src/tests/psi.cpp


    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

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

# Подключение пакета libmodbus
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBMODBUS REQUIRED libmodbus)

# Подключение библиотек GLFW и OpenGL
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# Создаем исполняемый файл
add_executable(modbus_rs485 ${SOURCES})

# Указываем пути для include файлов
target_include_directories(modbus_rs485 PRIVATE
    ${LIBMODBUS_INCLUDE_DIRS}
    imgui
    imgui/backends
    ${VISA_INCLUDE_DIR}
)

# Линкуем библиотеки
target_link_libraries(modbus_rs485 PRIVATE
    /usr/lib/x86_64-linux-gnu/libmodbus.so
    glfw
    OpenGL::GL
    ${VISA_LIBRARY}
)

# Указываем директорию для исполнимых файлов
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bins)

# 1. Копирование папок fonts и scenarios в bins
add_custom_target(copy-fonts ALL 
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/fonts ${CMAKE_BINARY_DIR}/bins/fonts
)

add_custom_target(copy-scenarios ALL 
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/scenarios ${CMAKE_BINARY_DIR}/bins/scenarios
)

set(XML_SOURCE_DIR "${CMAKE_SOURCE_DIR}/xml")
set(XML_DEST_DIR "${CMAKE_BINARY_DIR}/bins/xml")

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

