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
CMAKE_SOURCE_DIR = /home/kinova/Documents/kinovaZED

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kinova/Documents/kinovaZED/_build

# Include any dependencies generated for this target.
include src/CMakeFiles/kinovaArm_lib.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/kinovaArm_lib.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/kinovaArm_lib.dir/flags.make

src/CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.o: src/CMakeFiles/kinovaArm_lib.dir/flags.make
src/CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.o: ../src/KinovaArm.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kinova/Documents/kinovaZED/_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.o"
	cd /home/kinova/Documents/kinovaZED/_build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.o -c /home/kinova/Documents/kinovaZED/src/KinovaArm.cpp

src/CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.i"
	cd /home/kinova/Documents/kinovaZED/_build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kinova/Documents/kinovaZED/src/KinovaArm.cpp > CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.i

src/CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.s"
	cd /home/kinova/Documents/kinovaZED/_build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kinova/Documents/kinovaZED/src/KinovaArm.cpp -o CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.s

src/CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.o.requires:

.PHONY : src/CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.o.requires

src/CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.o.provides: src/CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/kinovaArm_lib.dir/build.make src/CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.o.provides.build
.PHONY : src/CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.o.provides

src/CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.o.provides.build: src/CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.o


src/CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.o: src/CMakeFiles/kinovaArm_lib.dir/flags.make
src/CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.o: ../src/TCPServer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kinova/Documents/kinovaZED/_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.o"
	cd /home/kinova/Documents/kinovaZED/_build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.o -c /home/kinova/Documents/kinovaZED/src/TCPServer.cpp

src/CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.i"
	cd /home/kinova/Documents/kinovaZED/_build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kinova/Documents/kinovaZED/src/TCPServer.cpp > CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.i

src/CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.s"
	cd /home/kinova/Documents/kinovaZED/_build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kinova/Documents/kinovaZED/src/TCPServer.cpp -o CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.s

src/CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.o.requires:

.PHONY : src/CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.o.requires

src/CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.o.provides: src/CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/kinovaArm_lib.dir/build.make src/CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.o.provides.build
.PHONY : src/CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.o.provides

src/CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.o.provides.build: src/CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.o


src/CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.o: src/CMakeFiles/kinovaArm_lib.dir/flags.make
src/CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.o: ../src/PositionHandling.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kinova/Documents/kinovaZED/_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.o"
	cd /home/kinova/Documents/kinovaZED/_build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.o -c /home/kinova/Documents/kinovaZED/src/PositionHandling.cpp

src/CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.i"
	cd /home/kinova/Documents/kinovaZED/_build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kinova/Documents/kinovaZED/src/PositionHandling.cpp > CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.i

src/CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.s"
	cd /home/kinova/Documents/kinovaZED/_build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kinova/Documents/kinovaZED/src/PositionHandling.cpp -o CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.s

src/CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.o.requires:

.PHONY : src/CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.o.requires

src/CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.o.provides: src/CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/kinovaArm_lib.dir/build.make src/CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.o.provides.build
.PHONY : src/CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.o.provides

src/CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.o.provides.build: src/CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.o


src/CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.o: src/CMakeFiles/kinovaArm_lib.dir/flags.make
src/CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.o: ../src/StateMachine.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kinova/Documents/kinovaZED/_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object src/CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.o"
	cd /home/kinova/Documents/kinovaZED/_build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.o -c /home/kinova/Documents/kinovaZED/src/StateMachine.cpp

src/CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.i"
	cd /home/kinova/Documents/kinovaZED/_build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kinova/Documents/kinovaZED/src/StateMachine.cpp > CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.i

src/CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.s"
	cd /home/kinova/Documents/kinovaZED/_build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kinova/Documents/kinovaZED/src/StateMachine.cpp -o CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.s

src/CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.o.requires:

.PHONY : src/CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.o.requires

src/CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.o.provides: src/CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/kinovaArm_lib.dir/build.make src/CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.o.provides.build
.PHONY : src/CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.o.provides

src/CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.o.provides.build: src/CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.o


src/CMakeFiles/kinovaArm_lib.dir/State.cpp.o: src/CMakeFiles/kinovaArm_lib.dir/flags.make
src/CMakeFiles/kinovaArm_lib.dir/State.cpp.o: ../src/State.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kinova/Documents/kinovaZED/_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object src/CMakeFiles/kinovaArm_lib.dir/State.cpp.o"
	cd /home/kinova/Documents/kinovaZED/_build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kinovaArm_lib.dir/State.cpp.o -c /home/kinova/Documents/kinovaZED/src/State.cpp

src/CMakeFiles/kinovaArm_lib.dir/State.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kinovaArm_lib.dir/State.cpp.i"
	cd /home/kinova/Documents/kinovaZED/_build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kinova/Documents/kinovaZED/src/State.cpp > CMakeFiles/kinovaArm_lib.dir/State.cpp.i

src/CMakeFiles/kinovaArm_lib.dir/State.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kinovaArm_lib.dir/State.cpp.s"
	cd /home/kinova/Documents/kinovaZED/_build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kinova/Documents/kinovaZED/src/State.cpp -o CMakeFiles/kinovaArm_lib.dir/State.cpp.s

src/CMakeFiles/kinovaArm_lib.dir/State.cpp.o.requires:

.PHONY : src/CMakeFiles/kinovaArm_lib.dir/State.cpp.o.requires

src/CMakeFiles/kinovaArm_lib.dir/State.cpp.o.provides: src/CMakeFiles/kinovaArm_lib.dir/State.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/kinovaArm_lib.dir/build.make src/CMakeFiles/kinovaArm_lib.dir/State.cpp.o.provides.build
.PHONY : src/CMakeFiles/kinovaArm_lib.dir/State.cpp.o.provides

src/CMakeFiles/kinovaArm_lib.dir/State.cpp.o.provides.build: src/CMakeFiles/kinovaArm_lib.dir/State.cpp.o


# Object files for target kinovaArm_lib
kinovaArm_lib_OBJECTS = \
"CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.o" \
"CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.o" \
"CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.o" \
"CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.o" \
"CMakeFiles/kinovaArm_lib.dir/State.cpp.o"

# External object files for target kinovaArm_lib
kinovaArm_lib_EXTERNAL_OBJECTS =

src/libkinovaArm_lib.a: src/CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.o
src/libkinovaArm_lib.a: src/CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.o
src/libkinovaArm_lib.a: src/CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.o
src/libkinovaArm_lib.a: src/CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.o
src/libkinovaArm_lib.a: src/CMakeFiles/kinovaArm_lib.dir/State.cpp.o
src/libkinovaArm_lib.a: src/CMakeFiles/kinovaArm_lib.dir/build.make
src/libkinovaArm_lib.a: src/CMakeFiles/kinovaArm_lib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/kinova/Documents/kinovaZED/_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX static library libkinovaArm_lib.a"
	cd /home/kinova/Documents/kinovaZED/_build/src && $(CMAKE_COMMAND) -P CMakeFiles/kinovaArm_lib.dir/cmake_clean_target.cmake
	cd /home/kinova/Documents/kinovaZED/_build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/kinovaArm_lib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/kinovaArm_lib.dir/build: src/libkinovaArm_lib.a

.PHONY : src/CMakeFiles/kinovaArm_lib.dir/build

src/CMakeFiles/kinovaArm_lib.dir/requires: src/CMakeFiles/kinovaArm_lib.dir/KinovaArm.cpp.o.requires
src/CMakeFiles/kinovaArm_lib.dir/requires: src/CMakeFiles/kinovaArm_lib.dir/TCPServer.cpp.o.requires
src/CMakeFiles/kinovaArm_lib.dir/requires: src/CMakeFiles/kinovaArm_lib.dir/PositionHandling.cpp.o.requires
src/CMakeFiles/kinovaArm_lib.dir/requires: src/CMakeFiles/kinovaArm_lib.dir/StateMachine.cpp.o.requires
src/CMakeFiles/kinovaArm_lib.dir/requires: src/CMakeFiles/kinovaArm_lib.dir/State.cpp.o.requires

.PHONY : src/CMakeFiles/kinovaArm_lib.dir/requires

src/CMakeFiles/kinovaArm_lib.dir/clean:
	cd /home/kinova/Documents/kinovaZED/_build/src && $(CMAKE_COMMAND) -P CMakeFiles/kinovaArm_lib.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/kinovaArm_lib.dir/clean

src/CMakeFiles/kinovaArm_lib.dir/depend:
	cd /home/kinova/Documents/kinovaZED/_build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kinova/Documents/kinovaZED /home/kinova/Documents/kinovaZED/src /home/kinova/Documents/kinovaZED/_build /home/kinova/Documents/kinovaZED/_build/src /home/kinova/Documents/kinovaZED/_build/src/CMakeFiles/kinovaArm_lib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/kinovaArm_lib.dir/depend

