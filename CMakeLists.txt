# User config between these lines
# ------------------------------------------

# Set project name
set(PROJECT "stowage")
# Set debugging, ON/OFF
set(DEBUG "ON")

# ------------------------------------------

# Add 'make run' target
add_custom_target(run
    COMMAND ${PROJECT}
)

# ------------------------------------------

cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Check if the build should include debugging symbols
option(DEBUG "" ${DEBUG})
if(DEBUG)
	# cmake -DDEBUG=on .. && make
	message("--- Debug ---")
	set(CMAKE_BUILD_TYPE "Debug")

	# -Og     = Optimizations that do not interfere with debugging
	# -Wall   = All warnings about contructions that are easily avoidable
	# -Wextra = Extra warning flags not covered by -Wall
	# -g      = Produce debugging information in OS's native format
	# -pg     = Generate profile information for analysis with gprof
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -Wall -Wextra -g -pg")
	# gprof <PROJECT> gmon.out > profile-data.txt
else()
	# cmake -DDEBUG=off .. && make
	message("--- Release ---")
	set(CMAKE_BUILD_TYPE "Release")

	# -O3     = Optimizations that increases compilation time and performance
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

# Include all headers
include_directories(
	"src"
	"test"
)

# Define source files
file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp")
set(PROJECT_SOURCES ${PROJECT_SOURCES})

# Define test source files
file(GLOB_RECURSE TEST_SOURCES "test/*.cpp")
file(GLOB_RECURSE MAIN_SOURCES "src/*/*.cpp")
set(TEST_SOURCES ${TEST_SOURCES} ${MAIN_SOURCES})

# ------------------------------------------

project(${PROJECT})
set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT} ${PROJECT_SOURCES})
target_link_libraries(${PROJECT})

# ------------------------------------------

project(test)
set(CMAKE_CXX_STANDARD 17)

add_executable(test ${TEST_SOURCES})
target_link_libraries(test)
