cmake_minimum_required(VERSION 3.16)

project(tablo)

set(CMAKE_CXX_STANDARD 17)   

# Устанавливаем файл bmp.c как C-файл
set_source_files_properties(bmp.c PROPERTIES LANGUAGE C)

# Указываем путь к папке imgui
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/imgui")

# Собираем файлы ImGui
file(GLOB IMGUI_SOURCES
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)

# Добавляем исполняемый файл и указываем исходные файлы
add_executable(${PROJECT_NAME} 
                main.cpp
                image.cpp
                bmp.c
                ftfont.c
                panels.cpp
                socket.cpp
                utils.cpp
                ${IMGUI_SOURCES}
)

# Указываем путь к заголовочным файлам ImGui и libusb
target_include_directories(${PROJECT_NAME} PRIVATE 
                           ${IMGUI_DIR} 
                           ${IMGUI_DIR}/backends 
                           /usr/include
                           /usr/include/freetype2)

# Связываем проект с библиотеками libusb-1.0, OpenGL, GLFW, Curses и Freetype
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(Freetype REQUIRED)
if (Freetype_FOUND)
    message(STATUS "Freetype найден")
else()
    message(FATAL_ERROR "Freetype не найден")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE glfw OpenGL::GL ${CURSES_LIBRARIES} Freetype::Freetype)
target_link_libraries(glfw)
# Подключаем заголовочные файлы Curses
target_include_directories(${PROJECT_NAME} PRIVATE ${CURSES_INCLUDE_DIR})
