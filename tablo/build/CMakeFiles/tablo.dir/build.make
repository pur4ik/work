# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/pur4ik/Desktop/in work"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/pur4ik/Desktop/in work/build"

# Include any dependencies generated for this target.
include CMakeFiles/tablo.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/tablo.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/tablo.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tablo.dir/flags.make

CMakeFiles/tablo.dir/main.cpp.o: CMakeFiles/tablo.dir/flags.make
CMakeFiles/tablo.dir/main.cpp.o: /home/pur4ik/Desktop/in\ work/main.cpp
CMakeFiles/tablo.dir/main.cpp.o: CMakeFiles/tablo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/home/pur4ik/Desktop/in work/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/tablo.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tablo.dir/main.cpp.o -MF CMakeFiles/tablo.dir/main.cpp.o.d -o CMakeFiles/tablo.dir/main.cpp.o -c "/home/pur4ik/Desktop/in work/main.cpp"

CMakeFiles/tablo.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/tablo.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/pur4ik/Desktop/in work/main.cpp" > CMakeFiles/tablo.dir/main.cpp.i

CMakeFiles/tablo.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/tablo.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/pur4ik/Desktop/in work/main.cpp" -o CMakeFiles/tablo.dir/main.cpp.s

CMakeFiles/tablo.dir/image.cpp.o: CMakeFiles/tablo.dir/flags.make
CMakeFiles/tablo.dir/image.cpp.o: /home/pur4ik/Desktop/in\ work/image.cpp
CMakeFiles/tablo.dir/image.cpp.o: CMakeFiles/tablo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/home/pur4ik/Desktop/in work/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/tablo.dir/image.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tablo.dir/image.cpp.o -MF CMakeFiles/tablo.dir/image.cpp.o.d -o CMakeFiles/tablo.dir/image.cpp.o -c "/home/pur4ik/Desktop/in work/image.cpp"

CMakeFiles/tablo.dir/image.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/tablo.dir/image.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/pur4ik/Desktop/in work/image.cpp" > CMakeFiles/tablo.dir/image.cpp.i

CMakeFiles/tablo.dir/image.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/tablo.dir/image.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/pur4ik/Desktop/in work/image.cpp" -o CMakeFiles/tablo.dir/image.cpp.s

CMakeFiles/tablo.dir/bmp.c.o: CMakeFiles/tablo.dir/flags.make
CMakeFiles/tablo.dir/bmp.c.o: /home/pur4ik/Desktop/in\ work/bmp.c
CMakeFiles/tablo.dir/bmp.c.o: CMakeFiles/tablo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/home/pur4ik/Desktop/in work/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/tablo.dir/bmp.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/tablo.dir/bmp.c.o -MF CMakeFiles/tablo.dir/bmp.c.o.d -o CMakeFiles/tablo.dir/bmp.c.o -c "/home/pur4ik/Desktop/in work/bmp.c"

CMakeFiles/tablo.dir/bmp.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/tablo.dir/bmp.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/pur4ik/Desktop/in work/bmp.c" > CMakeFiles/tablo.dir/bmp.c.i

CMakeFiles/tablo.dir/bmp.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/tablo.dir/bmp.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/pur4ik/Desktop/in work/bmp.c" -o CMakeFiles/tablo.dir/bmp.c.s

CMakeFiles/tablo.dir/ftfont.c.o: CMakeFiles/tablo.dir/flags.make
CMakeFiles/tablo.dir/ftfont.c.o: /home/pur4ik/Desktop/in\ work/ftfont.c
CMakeFiles/tablo.dir/ftfont.c.o: CMakeFiles/tablo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/home/pur4ik/Desktop/in work/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/tablo.dir/ftfont.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/tablo.dir/ftfont.c.o -MF CMakeFiles/tablo.dir/ftfont.c.o.d -o CMakeFiles/tablo.dir/ftfont.c.o -c "/home/pur4ik/Desktop/in work/ftfont.c"

CMakeFiles/tablo.dir/ftfont.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/tablo.dir/ftfont.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/pur4ik/Desktop/in work/ftfont.c" > CMakeFiles/tablo.dir/ftfont.c.i

CMakeFiles/tablo.dir/ftfont.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/tablo.dir/ftfont.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/pur4ik/Desktop/in work/ftfont.c" -o CMakeFiles/tablo.dir/ftfont.c.s

CMakeFiles/tablo.dir/panels.cpp.o: CMakeFiles/tablo.dir/flags.make
CMakeFiles/tablo.dir/panels.cpp.o: /home/pur4ik/Desktop/in\ work/panels.cpp
CMakeFiles/tablo.dir/panels.cpp.o: CMakeFiles/tablo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/home/pur4ik/Desktop/in work/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/tablo.dir/panels.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tablo.dir/panels.cpp.o -MF CMakeFiles/tablo.dir/panels.cpp.o.d -o CMakeFiles/tablo.dir/panels.cpp.o -c "/home/pur4ik/Desktop/in work/panels.cpp"

CMakeFiles/tablo.dir/panels.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/tablo.dir/panels.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/pur4ik/Desktop/in work/panels.cpp" > CMakeFiles/tablo.dir/panels.cpp.i

CMakeFiles/tablo.dir/panels.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/tablo.dir/panels.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/pur4ik/Desktop/in work/panels.cpp" -o CMakeFiles/tablo.dir/panels.cpp.s

CMakeFiles/tablo.dir/socket.cpp.o: CMakeFiles/tablo.dir/flags.make
CMakeFiles/tablo.dir/socket.cpp.o: /home/pur4ik/Desktop/in\ work/socket.cpp
CMakeFiles/tablo.dir/socket.cpp.o: CMakeFiles/tablo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/home/pur4ik/Desktop/in work/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/tablo.dir/socket.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tablo.dir/socket.cpp.o -MF CMakeFiles/tablo.dir/socket.cpp.o.d -o CMakeFiles/tablo.dir/socket.cpp.o -c "/home/pur4ik/Desktop/in work/socket.cpp"

CMakeFiles/tablo.dir/socket.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/tablo.dir/socket.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/pur4ik/Desktop/in work/socket.cpp" > CMakeFiles/tablo.dir/socket.cpp.i

CMakeFiles/tablo.dir/socket.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/tablo.dir/socket.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/pur4ik/Desktop/in work/socket.cpp" -o CMakeFiles/tablo.dir/socket.cpp.s

CMakeFiles/tablo.dir/utils.cpp.o: CMakeFiles/tablo.dir/flags.make
CMakeFiles/tablo.dir/utils.cpp.o: /home/pur4ik/Desktop/in\ work/utils.cpp
CMakeFiles/tablo.dir/utils.cpp.o: CMakeFiles/tablo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/home/pur4ik/Desktop/in work/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/tablo.dir/utils.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tablo.dir/utils.cpp.o -MF CMakeFiles/tablo.dir/utils.cpp.o.d -o CMakeFiles/tablo.dir/utils.cpp.o -c "/home/pur4ik/Desktop/in work/utils.cpp"

CMakeFiles/tablo.dir/utils.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/tablo.dir/utils.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/pur4ik/Desktop/in work/utils.cpp" > CMakeFiles/tablo.dir/utils.cpp.i

CMakeFiles/tablo.dir/utils.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/tablo.dir/utils.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/pur4ik/Desktop/in work/utils.cpp" -o CMakeFiles/tablo.dir/utils.cpp.s

CMakeFiles/tablo.dir/imgui/backends/imgui_impl_glfw.cpp.o: CMakeFiles/tablo.dir/flags.make
CMakeFiles/tablo.dir/imgui/backends/imgui_impl_glfw.cpp.o: /home/pur4ik/Desktop/in\ work/imgui/backends/imgui_impl_glfw.cpp
CMakeFiles/tablo.dir/imgui/backends/imgui_impl_glfw.cpp.o: CMakeFiles/tablo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/home/pur4ik/Desktop/in work/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/tablo.dir/imgui/backends/imgui_impl_glfw.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tablo.dir/imgui/backends/imgui_impl_glfw.cpp.o -MF CMakeFiles/tablo.dir/imgui/backends/imgui_impl_glfw.cpp.o.d -o CMakeFiles/tablo.dir/imgui/backends/imgui_impl_glfw.cpp.o -c "/home/pur4ik/Desktop/in work/imgui/backends/imgui_impl_glfw.cpp"

CMakeFiles/tablo.dir/imgui/backends/imgui_impl_glfw.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/tablo.dir/imgui/backends/imgui_impl_glfw.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/pur4ik/Desktop/in work/imgui/backends/imgui_impl_glfw.cpp" > CMakeFiles/tablo.dir/imgui/backends/imgui_impl_glfw.cpp.i

CMakeFiles/tablo.dir/imgui/backends/imgui_impl_glfw.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/tablo.dir/imgui/backends/imgui_impl_glfw.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/pur4ik/Desktop/in work/imgui/backends/imgui_impl_glfw.cpp" -o CMakeFiles/tablo.dir/imgui/backends/imgui_impl_glfw.cpp.s

CMakeFiles/tablo.dir/imgui/backends/imgui_impl_opengl3.cpp.o: CMakeFiles/tablo.dir/flags.make
CMakeFiles/tablo.dir/imgui/backends/imgui_impl_opengl3.cpp.o: /home/pur4ik/Desktop/in\ work/imgui/backends/imgui_impl_opengl3.cpp
CMakeFiles/tablo.dir/imgui/backends/imgui_impl_opengl3.cpp.o: CMakeFiles/tablo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/home/pur4ik/Desktop/in work/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/tablo.dir/imgui/backends/imgui_impl_opengl3.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tablo.dir/imgui/backends/imgui_impl_opengl3.cpp.o -MF CMakeFiles/tablo.dir/imgui/backends/imgui_impl_opengl3.cpp.o.d -o CMakeFiles/tablo.dir/imgui/backends/imgui_impl_opengl3.cpp.o -c "/home/pur4ik/Desktop/in work/imgui/backends/imgui_impl_opengl3.cpp"

CMakeFiles/tablo.dir/imgui/backends/imgui_impl_opengl3.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/tablo.dir/imgui/backends/imgui_impl_opengl3.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/pur4ik/Desktop/in work/imgui/backends/imgui_impl_opengl3.cpp" > CMakeFiles/tablo.dir/imgui/backends/imgui_impl_opengl3.cpp.i

CMakeFiles/tablo.dir/imgui/backends/imgui_impl_opengl3.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/tablo.dir/imgui/backends/imgui_impl_opengl3.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/pur4ik/Desktop/in work/imgui/backends/imgui_impl_opengl3.cpp" -o CMakeFiles/tablo.dir/imgui/backends/imgui_impl_opengl3.cpp.s

CMakeFiles/tablo.dir/imgui/imgui.cpp.o: CMakeFiles/tablo.dir/flags.make
CMakeFiles/tablo.dir/imgui/imgui.cpp.o: /home/pur4ik/Desktop/in\ work/imgui/imgui.cpp
CMakeFiles/tablo.dir/imgui/imgui.cpp.o: CMakeFiles/tablo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/home/pur4ik/Desktop/in work/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/tablo.dir/imgui/imgui.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tablo.dir/imgui/imgui.cpp.o -MF CMakeFiles/tablo.dir/imgui/imgui.cpp.o.d -o CMakeFiles/tablo.dir/imgui/imgui.cpp.o -c "/home/pur4ik/Desktop/in work/imgui/imgui.cpp"

CMakeFiles/tablo.dir/imgui/imgui.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/tablo.dir/imgui/imgui.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/pur4ik/Desktop/in work/imgui/imgui.cpp" > CMakeFiles/tablo.dir/imgui/imgui.cpp.i

CMakeFiles/tablo.dir/imgui/imgui.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/tablo.dir/imgui/imgui.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/pur4ik/Desktop/in work/imgui/imgui.cpp" -o CMakeFiles/tablo.dir/imgui/imgui.cpp.s

CMakeFiles/tablo.dir/imgui/imgui_draw.cpp.o: CMakeFiles/tablo.dir/flags.make
CMakeFiles/tablo.dir/imgui/imgui_draw.cpp.o: /home/pur4ik/Desktop/in\ work/imgui/imgui_draw.cpp
CMakeFiles/tablo.dir/imgui/imgui_draw.cpp.o: CMakeFiles/tablo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/home/pur4ik/Desktop/in work/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/tablo.dir/imgui/imgui_draw.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tablo.dir/imgui/imgui_draw.cpp.o -MF CMakeFiles/tablo.dir/imgui/imgui_draw.cpp.o.d -o CMakeFiles/tablo.dir/imgui/imgui_draw.cpp.o -c "/home/pur4ik/Desktop/in work/imgui/imgui_draw.cpp"

CMakeFiles/tablo.dir/imgui/imgui_draw.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/tablo.dir/imgui/imgui_draw.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/pur4ik/Desktop/in work/imgui/imgui_draw.cpp" > CMakeFiles/tablo.dir/imgui/imgui_draw.cpp.i

CMakeFiles/tablo.dir/imgui/imgui_draw.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/tablo.dir/imgui/imgui_draw.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/pur4ik/Desktop/in work/imgui/imgui_draw.cpp" -o CMakeFiles/tablo.dir/imgui/imgui_draw.cpp.s

CMakeFiles/tablo.dir/imgui/imgui_tables.cpp.o: CMakeFiles/tablo.dir/flags.make
CMakeFiles/tablo.dir/imgui/imgui_tables.cpp.o: /home/pur4ik/Desktop/in\ work/imgui/imgui_tables.cpp
CMakeFiles/tablo.dir/imgui/imgui_tables.cpp.o: CMakeFiles/tablo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/home/pur4ik/Desktop/in work/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object CMakeFiles/tablo.dir/imgui/imgui_tables.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tablo.dir/imgui/imgui_tables.cpp.o -MF CMakeFiles/tablo.dir/imgui/imgui_tables.cpp.o.d -o CMakeFiles/tablo.dir/imgui/imgui_tables.cpp.o -c "/home/pur4ik/Desktop/in work/imgui/imgui_tables.cpp"

CMakeFiles/tablo.dir/imgui/imgui_tables.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/tablo.dir/imgui/imgui_tables.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/pur4ik/Desktop/in work/imgui/imgui_tables.cpp" > CMakeFiles/tablo.dir/imgui/imgui_tables.cpp.i

CMakeFiles/tablo.dir/imgui/imgui_tables.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/tablo.dir/imgui/imgui_tables.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/pur4ik/Desktop/in work/imgui/imgui_tables.cpp" -o CMakeFiles/tablo.dir/imgui/imgui_tables.cpp.s

CMakeFiles/tablo.dir/imgui/imgui_widgets.cpp.o: CMakeFiles/tablo.dir/flags.make
CMakeFiles/tablo.dir/imgui/imgui_widgets.cpp.o: /home/pur4ik/Desktop/in\ work/imgui/imgui_widgets.cpp
CMakeFiles/tablo.dir/imgui/imgui_widgets.cpp.o: CMakeFiles/tablo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/home/pur4ik/Desktop/in work/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object CMakeFiles/tablo.dir/imgui/imgui_widgets.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tablo.dir/imgui/imgui_widgets.cpp.o -MF CMakeFiles/tablo.dir/imgui/imgui_widgets.cpp.o.d -o CMakeFiles/tablo.dir/imgui/imgui_widgets.cpp.o -c "/home/pur4ik/Desktop/in work/imgui/imgui_widgets.cpp"

CMakeFiles/tablo.dir/imgui/imgui_widgets.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/tablo.dir/imgui/imgui_widgets.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/pur4ik/Desktop/in work/imgui/imgui_widgets.cpp" > CMakeFiles/tablo.dir/imgui/imgui_widgets.cpp.i

CMakeFiles/tablo.dir/imgui/imgui_widgets.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/tablo.dir/imgui/imgui_widgets.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/pur4ik/Desktop/in work/imgui/imgui_widgets.cpp" -o CMakeFiles/tablo.dir/imgui/imgui_widgets.cpp.s

# Object files for target tablo
tablo_OBJECTS = \
"CMakeFiles/tablo.dir/main.cpp.o" \
"CMakeFiles/tablo.dir/image.cpp.o" \
"CMakeFiles/tablo.dir/bmp.c.o" \
"CMakeFiles/tablo.dir/ftfont.c.o" \
"CMakeFiles/tablo.dir/panels.cpp.o" \
"CMakeFiles/tablo.dir/socket.cpp.o" \
"CMakeFiles/tablo.dir/utils.cpp.o" \
"CMakeFiles/tablo.dir/imgui/backends/imgui_impl_glfw.cpp.o" \
"CMakeFiles/tablo.dir/imgui/backends/imgui_impl_opengl3.cpp.o" \
"CMakeFiles/tablo.dir/imgui/imgui.cpp.o" \
"CMakeFiles/tablo.dir/imgui/imgui_draw.cpp.o" \
"CMakeFiles/tablo.dir/imgui/imgui_tables.cpp.o" \
"CMakeFiles/tablo.dir/imgui/imgui_widgets.cpp.o"

# External object files for target tablo
tablo_EXTERNAL_OBJECTS =

tablo: CMakeFiles/tablo.dir/main.cpp.o
tablo: CMakeFiles/tablo.dir/image.cpp.o
tablo: CMakeFiles/tablo.dir/bmp.c.o
tablo: CMakeFiles/tablo.dir/ftfont.c.o
tablo: CMakeFiles/tablo.dir/panels.cpp.o
tablo: CMakeFiles/tablo.dir/socket.cpp.o
tablo: CMakeFiles/tablo.dir/utils.cpp.o
tablo: CMakeFiles/tablo.dir/imgui/backends/imgui_impl_glfw.cpp.o
tablo: CMakeFiles/tablo.dir/imgui/backends/imgui_impl_opengl3.cpp.o
tablo: CMakeFiles/tablo.dir/imgui/imgui.cpp.o
tablo: CMakeFiles/tablo.dir/imgui/imgui_draw.cpp.o
tablo: CMakeFiles/tablo.dir/imgui/imgui_tables.cpp.o
tablo: CMakeFiles/tablo.dir/imgui/imgui_widgets.cpp.o
tablo: CMakeFiles/tablo.dir/build.make
tablo: /usr/lib/x86_64-linux-gnu/libglfw.so.3.4
tablo: /usr/lib/x86_64-linux-gnu/libfreetype.so
tablo: /usr/lib/x86_64-linux-gnu/libGLX.so
tablo: /usr/lib/x86_64-linux-gnu/libOpenGL.so
tablo: CMakeFiles/tablo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="/home/pur4ik/Desktop/in work/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_14) "Linking CXX executable tablo"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tablo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tablo.dir/build: tablo
.PHONY : CMakeFiles/tablo.dir/build

CMakeFiles/tablo.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tablo.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tablo.dir/clean

CMakeFiles/tablo.dir/depend:
	cd "/home/pur4ik/Desktop/in work/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/pur4ik/Desktop/in work" "/home/pur4ik/Desktop/in work" "/home/pur4ik/Desktop/in work/build" "/home/pur4ik/Desktop/in work/build" "/home/pur4ik/Desktop/in work/build/CMakeFiles/tablo.dir/DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/tablo.dir/depend

