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
include thirdparty/jansson-2.12/CMakeFiles/test_object.dir/depend.make

# Include the progress variables for this target.
include thirdparty/jansson-2.12/CMakeFiles/test_object.dir/progress.make

# Include the compile flags for this target's objects.
include thirdparty/jansson-2.12/CMakeFiles/test_object.dir/flags.make

thirdparty/jansson-2.12/CMakeFiles/test_object.dir/test/suites/api/test_object.c.o: thirdparty/jansson-2.12/CMakeFiles/test_object.dir/flags.make
thirdparty/jansson-2.12/CMakeFiles/test_object.dir/test/suites/api/test_object.c.o: ../thirdparty/jansson-2.12/test/suites/api/test_object.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kinova/Documents/kinovaZED/_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object thirdparty/jansson-2.12/CMakeFiles/test_object.dir/test/suites/api/test_object.c.o"
	cd /home/kinova/Documents/kinovaZED/_build/thirdparty/jansson-2.12 && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/test_object.dir/test/suites/api/test_object.c.o   -c /home/kinova/Documents/kinovaZED/thirdparty/jansson-2.12/test/suites/api/test_object.c

thirdparty/jansson-2.12/CMakeFiles/test_object.dir/test/suites/api/test_object.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test_object.dir/test/suites/api/test_object.c.i"
	cd /home/kinova/Documents/kinovaZED/_build/thirdparty/jansson-2.12 && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kinova/Documents/kinovaZED/thirdparty/jansson-2.12/test/suites/api/test_object.c > CMakeFiles/test_object.dir/test/suites/api/test_object.c.i

thirdparty/jansson-2.12/CMakeFiles/test_object.dir/test/suites/api/test_object.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test_object.dir/test/suites/api/test_object.c.s"
	cd /home/kinova/Documents/kinovaZED/_build/thirdparty/jansson-2.12 && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kinova/Documents/kinovaZED/thirdparty/jansson-2.12/test/suites/api/test_object.c -o CMakeFiles/test_object.dir/test/suites/api/test_object.c.s

thirdparty/jansson-2.12/CMakeFiles/test_object.dir/test/suites/api/test_object.c.o.requires:

.PHONY : thirdparty/jansson-2.12/CMakeFiles/test_object.dir/test/suites/api/test_object.c.o.requires

thirdparty/jansson-2.12/CMakeFiles/test_object.dir/test/suites/api/test_object.c.o.provides: thirdparty/jansson-2.12/CMakeFiles/test_object.dir/test/suites/api/test_object.c.o.requires
	$(MAKE) -f thirdparty/jansson-2.12/CMakeFiles/test_object.dir/build.make thirdparty/jansson-2.12/CMakeFiles/test_object.dir/test/suites/api/test_object.c.o.provides.build
.PHONY : thirdparty/jansson-2.12/CMakeFiles/test_object.dir/test/suites/api/test_object.c.o.provides

thirdparty/jansson-2.12/CMakeFiles/test_object.dir/test/suites/api/test_object.c.o.provides.build: thirdparty/jansson-2.12/CMakeFiles/test_object.dir/test/suites/api/test_object.c.o


# Object files for target test_object
test_object_OBJECTS = \
"CMakeFiles/test_object.dir/test/suites/api/test_object.c.o"

# External object files for target test_object
test_object_EXTERNAL_OBJECTS =

thirdparty/jansson-2.12/bin/test_object: thirdparty/jansson-2.12/CMakeFiles/test_object.dir/test/suites/api/test_object.c.o
thirdparty/jansson-2.12/bin/test_object: thirdparty/jansson-2.12/CMakeFiles/test_object.dir/build.make
thirdparty/jansson-2.12/bin/test_object: thirdparty/jansson-2.12/lib/libjansson.a
thirdparty/jansson-2.12/bin/test_object: thirdparty/jansson-2.12/CMakeFiles/test_object.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/kinova/Documents/kinovaZED/_build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable bin/test_object"
	cd /home/kinova/Documents/kinovaZED/_build/thirdparty/jansson-2.12 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_object.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
thirdparty/jansson-2.12/CMakeFiles/test_object.dir/build: thirdparty/jansson-2.12/bin/test_object

.PHONY : thirdparty/jansson-2.12/CMakeFiles/test_object.dir/build

thirdparty/jansson-2.12/CMakeFiles/test_object.dir/requires: thirdparty/jansson-2.12/CMakeFiles/test_object.dir/test/suites/api/test_object.c.o.requires

.PHONY : thirdparty/jansson-2.12/CMakeFiles/test_object.dir/requires

thirdparty/jansson-2.12/CMakeFiles/test_object.dir/clean:
	cd /home/kinova/Documents/kinovaZED/_build/thirdparty/jansson-2.12 && $(CMAKE_COMMAND) -P CMakeFiles/test_object.dir/cmake_clean.cmake
.PHONY : thirdparty/jansson-2.12/CMakeFiles/test_object.dir/clean

thirdparty/jansson-2.12/CMakeFiles/test_object.dir/depend:
	cd /home/kinova/Documents/kinovaZED/_build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kinova/Documents/kinovaZED /home/kinova/Documents/kinovaZED/thirdparty/jansson-2.12 /home/kinova/Documents/kinovaZED/_build /home/kinova/Documents/kinovaZED/_build/thirdparty/jansson-2.12 /home/kinova/Documents/kinovaZED/_build/thirdparty/jansson-2.12/CMakeFiles/test_object.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : thirdparty/jansson-2.12/CMakeFiles/test_object.dir/depend

