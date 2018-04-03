cmake_minimum_required(VERSION 3.2.2)
project(task1)

set(CMAKE_CXX_STANDARD 11)

include_directories(./)

set(CMAKE_CXX_FLAGS_DEBUG "-pipe -DDEBUG -Wall -ggdb -O0 ${CMAKE_CXX_FLAGS_ALL}")

set(CMAKE_CXX_FLAGS_RELEASE "-pipe -DNDEBUG -Wall -g0 -O2 -s ${CMAKE_CXX_FLAGS_ALL}")


set(SOURCE_FILES main.cpp)

add_executable(task1 ${SOURCE_FILES})
