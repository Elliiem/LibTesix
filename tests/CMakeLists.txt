cmake_minimum_required(VERSION 3.25)

project(tests)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES "main.cpp" "test.cpp")

add_executable(LibTesixTests ${SOURCES})
target_link_libraries(LibTesixTests PRIVATE LibTesix)

add_test(NAME test1 COMMAND LibTesixTests test1)