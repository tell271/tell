# https://stackoverflow.com/questions/14446495/cmake-project-structure-with-unit-tests

cmake_minimum_required(VERSION 2.8.9)

project(tell)

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -O3 -DNDEBUG")
set(CMAKE_BUILD_TYPE Release)

# from command line: cmake -DCMAKE_BUILD_TYPE=Debug 
# set(CMAKE_CXX_FLAGS "-std=c++17 -Wall")
# set(CMAKE_BUILD_TYPE Debug)

include_directories(.)

file(GLOB HEADERS "tell/*.h")
file(GLOB SOURCES "src/*.cc")

add_library(tell STATIC ${SOURCES})

install(FILES ${HEADERS} DESTINATION /usr/local/include/tell)
install(TARGETS tell DESTINATION /usr/local/lib)

enable_testing()
add_subdirectory(test)

