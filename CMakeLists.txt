cmake_minimum_required(VERSION 3.25)

project(LibTesix)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

add_definitions("-DNDEBUG")
#add_definitions("-DLIBTESIX_NO_SAFETY")

include_directories("${CMAKE_SOURCE_DIR}/include")

add_subdirectory(Examples)
