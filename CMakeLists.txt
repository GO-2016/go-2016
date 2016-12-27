cmake_minimum_required(VERSION 3.1)
project(go-2016)

# Enable C++11
set (CMAKE_CXX_STANDARD 11)

################################
# Find boost
################################
find_package(Boost 1.46 REQUIRED COMPONENTS program_options system)
include_directories(${Boost_INCLUDE_DIRS})

################################
# Find thread
################################
find_package(Threads REQUIRED)

################################
# GTest
################################
ADD_SUBDIRECTORY (vendor/googletest/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

################################
# gtplib
################################

set(gtplib_SOURCE_DIR vendor/gtplib)
find_package(Boost REQUIRED)
include_directories(${gtplib_SOURCE_DIR})

################################
# libgo-common
################################
add_subdirectory(${go-2016_SOURCE_DIR}/vendor/libgo-common)
include_directories(${libgo-common_INCLUDE_DIR})

################################
# libgoboard
################################
add_subdirectory(${go-2016_SOURCE_DIR}/vendor/libgoboard)
include_directories(${libgoboard_INCLUDE_DIR})

################################
# libmcts
################################
add_subdirectory(${go-2016_SOURCE_DIR}/vendor/libmcts)
include_directories(${libmcts_INCLUDE_DIR})

################################
# SRCS
################################
set(engines_SRC src/engines/engine_factory.hpp src/engines/engine_factory.cpp src/engines/simple_engine_base.hpp src/engines/simple_engine_base.cpp src/engines/rand_engine.hpp src/engines/uct_engine.hpp)

################################
# Main program
################################
include_directories(src/)
add_executable(go-2016 ${engines_SRC} src/main.cpp)
target_link_libraries(go-2016 ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY}  ${CMAKE_THREAD_LIBS_INIT} go-common goboard)

################################
# Unit Tests
################################
if (go-2016_build_tests)
    # Do nothing
    enable_testing()
endif()

