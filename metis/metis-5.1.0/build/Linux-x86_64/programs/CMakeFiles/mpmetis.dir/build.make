# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_SOURCE_DIR = /data3/DRILL/metis/metis-5.1.0

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64

# Include any dependencies generated for this target.
include programs/CMakeFiles/mpmetis.dir/depend.make

# Include the progress variables for this target.
include programs/CMakeFiles/mpmetis.dir/progress.make

# Include the compile flags for this target's objects.
include programs/CMakeFiles/mpmetis.dir/flags.make

programs/CMakeFiles/mpmetis.dir/mpmetis.c.o: programs/CMakeFiles/mpmetis.dir/flags.make
programs/CMakeFiles/mpmetis.dir/mpmetis.c.o: ../../programs/mpmetis.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object programs/CMakeFiles/mpmetis.dir/mpmetis.c.o"
	cd /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/programs && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/mpmetis.dir/mpmetis.c.o   -c /data3/DRILL/metis/metis-5.1.0/programs/mpmetis.c

programs/CMakeFiles/mpmetis.dir/mpmetis.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/mpmetis.dir/mpmetis.c.i"
	cd /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/programs && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /data3/DRILL/metis/metis-5.1.0/programs/mpmetis.c > CMakeFiles/mpmetis.dir/mpmetis.c.i

programs/CMakeFiles/mpmetis.dir/mpmetis.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/mpmetis.dir/mpmetis.c.s"
	cd /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/programs && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /data3/DRILL/metis/metis-5.1.0/programs/mpmetis.c -o CMakeFiles/mpmetis.dir/mpmetis.c.s

programs/CMakeFiles/mpmetis.dir/cmdline_mpmetis.c.o: programs/CMakeFiles/mpmetis.dir/flags.make
programs/CMakeFiles/mpmetis.dir/cmdline_mpmetis.c.o: ../../programs/cmdline_mpmetis.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object programs/CMakeFiles/mpmetis.dir/cmdline_mpmetis.c.o"
	cd /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/programs && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/mpmetis.dir/cmdline_mpmetis.c.o   -c /data3/DRILL/metis/metis-5.1.0/programs/cmdline_mpmetis.c

programs/CMakeFiles/mpmetis.dir/cmdline_mpmetis.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/mpmetis.dir/cmdline_mpmetis.c.i"
	cd /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/programs && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /data3/DRILL/metis/metis-5.1.0/programs/cmdline_mpmetis.c > CMakeFiles/mpmetis.dir/cmdline_mpmetis.c.i

programs/CMakeFiles/mpmetis.dir/cmdline_mpmetis.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/mpmetis.dir/cmdline_mpmetis.c.s"
	cd /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/programs && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /data3/DRILL/metis/metis-5.1.0/programs/cmdline_mpmetis.c -o CMakeFiles/mpmetis.dir/cmdline_mpmetis.c.s

programs/CMakeFiles/mpmetis.dir/io.c.o: programs/CMakeFiles/mpmetis.dir/flags.make
programs/CMakeFiles/mpmetis.dir/io.c.o: ../../programs/io.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object programs/CMakeFiles/mpmetis.dir/io.c.o"
	cd /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/programs && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/mpmetis.dir/io.c.o   -c /data3/DRILL/metis/metis-5.1.0/programs/io.c

programs/CMakeFiles/mpmetis.dir/io.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/mpmetis.dir/io.c.i"
	cd /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/programs && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /data3/DRILL/metis/metis-5.1.0/programs/io.c > CMakeFiles/mpmetis.dir/io.c.i

programs/CMakeFiles/mpmetis.dir/io.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/mpmetis.dir/io.c.s"
	cd /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/programs && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /data3/DRILL/metis/metis-5.1.0/programs/io.c -o CMakeFiles/mpmetis.dir/io.c.s

programs/CMakeFiles/mpmetis.dir/stat.c.o: programs/CMakeFiles/mpmetis.dir/flags.make
programs/CMakeFiles/mpmetis.dir/stat.c.o: ../../programs/stat.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object programs/CMakeFiles/mpmetis.dir/stat.c.o"
	cd /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/programs && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/mpmetis.dir/stat.c.o   -c /data3/DRILL/metis/metis-5.1.0/programs/stat.c

programs/CMakeFiles/mpmetis.dir/stat.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/mpmetis.dir/stat.c.i"
	cd /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/programs && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /data3/DRILL/metis/metis-5.1.0/programs/stat.c > CMakeFiles/mpmetis.dir/stat.c.i

programs/CMakeFiles/mpmetis.dir/stat.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/mpmetis.dir/stat.c.s"
	cd /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/programs && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /data3/DRILL/metis/metis-5.1.0/programs/stat.c -o CMakeFiles/mpmetis.dir/stat.c.s

# Object files for target mpmetis
mpmetis_OBJECTS = \
"CMakeFiles/mpmetis.dir/mpmetis.c.o" \
"CMakeFiles/mpmetis.dir/cmdline_mpmetis.c.o" \
"CMakeFiles/mpmetis.dir/io.c.o" \
"CMakeFiles/mpmetis.dir/stat.c.o"

# External object files for target mpmetis
mpmetis_EXTERNAL_OBJECTS =

programs/mpmetis: programs/CMakeFiles/mpmetis.dir/mpmetis.c.o
programs/mpmetis: programs/CMakeFiles/mpmetis.dir/cmdline_mpmetis.c.o
programs/mpmetis: programs/CMakeFiles/mpmetis.dir/io.c.o
programs/mpmetis: programs/CMakeFiles/mpmetis.dir/stat.c.o
programs/mpmetis: programs/CMakeFiles/mpmetis.dir/build.make
programs/mpmetis: libmetis/libmetis.so
programs/mpmetis: programs/CMakeFiles/mpmetis.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking C executable mpmetis"
	cd /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/programs && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mpmetis.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
programs/CMakeFiles/mpmetis.dir/build: programs/mpmetis

.PHONY : programs/CMakeFiles/mpmetis.dir/build

programs/CMakeFiles/mpmetis.dir/clean:
	cd /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/programs && $(CMAKE_COMMAND) -P CMakeFiles/mpmetis.dir/cmake_clean.cmake
.PHONY : programs/CMakeFiles/mpmetis.dir/clean

programs/CMakeFiles/mpmetis.dir/depend:
	cd /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data3/DRILL/metis/metis-5.1.0 /data3/DRILL/metis/metis-5.1.0/programs /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64 /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/programs /data3/DRILL/metis/metis-5.1.0/build/Linux-x86_64/programs/CMakeFiles/mpmetis.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : programs/CMakeFiles/mpmetis.dir/depend

