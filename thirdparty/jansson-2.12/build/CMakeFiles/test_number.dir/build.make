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
CMAKE_SOURCE_DIR = /home/kinova/Documents/kinovaZED/thirdparty/jansson-2.12

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kinova/Documents/kinovaZED/thirdparty/jansson-2.12/build

# Include any dependencies generated for this target.
include CMakeFiles/test_number.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_number.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_number.dir/flags.make

CMakeFiles/test_number.dir/test/suites/api/test_number.c.o: CMakeFiles/test_number.dir/flags.make
CMakeFiles/test_number.dir/test/suites/api/test_number.c.o: ../test/suites/api/test_number.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kinova/Documents/kinovaZED/thirdparty/jansson-2.12/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/test_number.dir/test/suites/api/test_number.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/test_number.dir/test/suites/api/test_number.c.o   -c /home/kinova/Documents/kinovaZED/thirdparty/jansson-2.12/test/suites/api/test_number.c

CMakeFiles/test_number.dir/test/suites/api/test_number.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test_number.dir/test/suites/api/test_number.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kinova/Documents/kinovaZED/thirdparty/jansson-2.12/test/suites/api/test_number.c > CMakeFiles/test_number.dir/test/suites/api/test_number.c.i

CMakeFiles/test_number.dir/test/suites/api/test_number.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test_number.dir/test/suites/api/test_number.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kinova/Documents/kinovaZED/thirdparty/jansson-2.12/test/suites/api/test_number.c -o CMakeFiles/test_number.dir/test/suites/api/test_number.c.s

CMakeFiles/test_number.dir/test/suites/api/test_number.c.o.requires:

.PHONY : CMakeFiles/test_number.dir/test/suites/api/test_number.c.o.requires

CMakeFiles/test_number.dir/test/suites/api/test_number.c.o.provides: CMakeFiles/test_number.dir/test/suites/api/test_number.c.o.requires
	$(MAKE) -f CMakeFiles/test_number.dir/build.make CMakeFiles/test_number.dir/test/suites/api/test_number.c.o.provides.build
.PHONY : CMakeFiles/test_number.dir/test/suites/api/test_number.c.o.provides

CMakeFiles/test_number.dir/test/suites/api/test_number.c.o.provides.build: CMakeFiles/test_number.dir/test/suites/api/test_number.c.o


# Object files for target test_number
test_number_OBJECTS = \
"CMakeFiles/test_number.dir/test/suites/api/test_number.c.o"

# External object files for target test_number
test_number_EXTERNAL_OBJECTS =

bin/test_number: CMakeFiles/test_number.dir/test/suites/api/test_number.c.o
bin/test_number: CMakeFiles/test_number.dir/build.make
bin/test_number: lib/libjansson.a
bin/test_number: CMakeFiles/test_number.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/kinova/Documents/kinovaZED/thirdparty/jansson-2.12/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable bin/test_number"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_number.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_number.dir/build: bin/test_number

.PHONY : CMakeFiles/test_number.dir/build

CMakeFiles/test_number.dir/requires: CMakeFiles/test_number.dir/test/suites/api/test_number.c.o.requires

.PHONY : CMakeFiles/test_number.dir/requires

CMakeFiles/test_number.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_number.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_number.dir/clean

CMakeFiles/test_number.dir/depend:
	cd /home/kinova/Documents/kinovaZED/thirdparty/jansson-2.12/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kinova/Documents/kinovaZED/thirdparty/jansson-2.12 /home/kinova/Documents/kinovaZED/thirdparty/jansson-2.12 /home/kinova/Documents/kinovaZED/thirdparty/jansson-2.12/build /home/kinova/Documents/kinovaZED/thirdparty/jansson-2.12/build /home/kinova/Documents/kinovaZED/thirdparty/jansson-2.12/build/CMakeFiles/test_number.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_number.dir/depend

