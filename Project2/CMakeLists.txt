cmake_minimum_required(VERSION 3.5)
project(Project2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES ftserver.c)
add_executable(Project2 ${SOURCE_FILES} ftserver.c)