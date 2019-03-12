# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/kinova/Documents/kinovazed

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kinova/Documents/kinovazed/build

# Include any dependencies generated for this target.
include src/CMakeFiles/KinovaZED.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/KinovaZED.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/KinovaZED.dir/flags.make

src/CMakeFiles/KinovaZED.dir/KinovaMain.cpp.o: src/CMakeFiles/KinovaZED.dir/flags.make
src/CMakeFiles/KinovaZED.dir/KinovaMain.cpp.o: ../src/KinovaMain.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kinova/Documents/kinovazed/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/KinovaZED.dir/KinovaMain.cpp.o"
	cd /home/kinova/Documents/kinovazed/build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/KinovaZED.dir/KinovaMain.cpp.o -c /home/kinova/Documents/kinovazed/src/KinovaMain.cpp

src/CMakeFiles/KinovaZED.dir/KinovaMain.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/KinovaZED.dir/KinovaMain.cpp.i"
	cd /home/kinova/Documents/kinovazed/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kinova/Documents/kinovazed/src/KinovaMain.cpp > CMakeFiles/KinovaZED.dir/KinovaMain.cpp.i

src/CMakeFiles/KinovaZED.dir/KinovaMain.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/KinovaZED.dir/KinovaMain.cpp.s"
	cd /home/kinova/Documents/kinovazed/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kinova/Documents/kinovazed/src/KinovaMain.cpp -o CMakeFiles/KinovaZED.dir/KinovaMain.cpp.s

src/CMakeFiles/KinovaZED.dir/KinovaMain.cpp.o.requires:

.PHONY : src/CMakeFiles/KinovaZED.dir/KinovaMain.cpp.o.requires

src/CMakeFiles/KinovaZED.dir/KinovaMain.cpp.o.provides: src/CMakeFiles/KinovaZED.dir/KinovaMain.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/KinovaZED.dir/build.make src/CMakeFiles/KinovaZED.dir/KinovaMain.cpp.o.provides.build
.PHONY : src/CMakeFiles/KinovaZED.dir/KinovaMain.cpp.o.provides

src/CMakeFiles/KinovaZED.dir/KinovaMain.cpp.o.provides.build: src/CMakeFiles/KinovaZED.dir/KinovaMain.cpp.o


# Object files for target KinovaZED
KinovaZED_OBJECTS = \
"CMakeFiles/KinovaZED.dir/KinovaMain.cpp.o"

# External object files for target KinovaZED
KinovaZED_EXTERNAL_OBJECTS =

src/KinovaZED: src/CMakeFiles/KinovaZED.dir/KinovaMain.cpp.o
src/KinovaZED: src/CMakeFiles/KinovaZED.dir/build.make
src/KinovaZED: src/libkinovaArm_lib.a
src/KinovaZED: /usr/local/lib/libkindrv/liblibkindrv.so
src/KinovaZED: /usr/lib/arm-linux-gnueabihf/libboost_system.so
src/KinovaZED: src/CMakeFiles/KinovaZED.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/kinova/Documents/kinovazed/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable KinovaZED"
	cd /home/kinova/Documents/kinovazed/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/KinovaZED.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/KinovaZED.dir/build: src/KinovaZED

.PHONY : src/CMakeFiles/KinovaZED.dir/build

src/CMakeFiles/KinovaZED.dir/requires: src/CMakeFiles/KinovaZED.dir/KinovaMain.cpp.o.requires

.PHONY : src/CMakeFiles/KinovaZED.dir/requires

src/CMakeFiles/KinovaZED.dir/clean:
	cd /home/kinova/Documents/kinovazed/build/src && $(CMAKE_COMMAND) -P CMakeFiles/KinovaZED.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/KinovaZED.dir/clean

src/CMakeFiles/KinovaZED.dir/depend:
	cd /home/kinova/Documents/kinovazed/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kinova/Documents/kinovazed /home/kinova/Documents/kinovazed/src /home/kinova/Documents/kinovazed/build /home/kinova/Documents/kinovazed/build/src /home/kinova/Documents/kinovazed/build/src/CMakeFiles/KinovaZED.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/KinovaZED.dir/depend

