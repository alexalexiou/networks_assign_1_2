# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/alex/CLionProjects/PrAssig2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/alex/CLionProjects/PrAssig2/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/helper.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/helper.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/helper.dir/flags.make

CMakeFiles/helper.dir/helper.c.o: CMakeFiles/helper.dir/flags.make
CMakeFiles/helper.dir/helper.c.o: ../helper.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/alex/CLionProjects/PrAssig2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/helper.dir/helper.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/helper.dir/helper.c.o   -c /Users/alex/CLionProjects/PrAssig2/helper.c

CMakeFiles/helper.dir/helper.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/helper.dir/helper.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/alex/CLionProjects/PrAssig2/helper.c > CMakeFiles/helper.dir/helper.c.i

CMakeFiles/helper.dir/helper.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/helper.dir/helper.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/alex/CLionProjects/PrAssig2/helper.c -o CMakeFiles/helper.dir/helper.c.s

# Object files for target helper
helper_OBJECTS = \
"CMakeFiles/helper.dir/helper.c.o"

# External object files for target helper
helper_EXTERNAL_OBJECTS =

helper: CMakeFiles/helper.dir/helper.c.o
helper: CMakeFiles/helper.dir/build.make
helper: CMakeFiles/helper.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/alex/CLionProjects/PrAssig2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable helper"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/helper.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/helper.dir/build: helper

.PHONY : CMakeFiles/helper.dir/build

CMakeFiles/helper.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/helper.dir/cmake_clean.cmake
.PHONY : CMakeFiles/helper.dir/clean

CMakeFiles/helper.dir/depend:
	cd /Users/alex/CLionProjects/PrAssig2/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/alex/CLionProjects/PrAssig2 /Users/alex/CLionProjects/PrAssig2 /Users/alex/CLionProjects/PrAssig2/cmake-build-debug /Users/alex/CLionProjects/PrAssig2/cmake-build-debug /Users/alex/CLionProjects/PrAssig2/cmake-build-debug/CMakeFiles/helper.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/helper.dir/depend

