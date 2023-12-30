cmake_minimum_required(VERSION 3.25)

project(LibTesix)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

option(COMPILE_EXAMPLES "aaa" ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

if(COMPILE_EXAMPLES)
    add_subdirectory(Examples)
endif()

find_package(ICU 72.1 COMPONENTS uc REQUIRED)

add_library(${PROJECT_NAME} STATIC
    ${SOURCES}
)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_23)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "libTesix")

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    ICU::uc
)

#set(SOURCES "./tests/main.cpp" "./tests/test.cpp")
#
#create_test_sourcelist (Tests
#    ${SOURCES}
#)
#
# add the executable
#add_executable (CommonCxxTests ${Tests})
#remove(Tests main.cpp)
#
# Add all the ADD_TEST for each test
#
#add_test (NAME ./tests/test COMMAND CommonCxxTests "./tests/test")
#
#set_property(TEST ./tests/test PROPERTY PASS_REGULAR_EXPRESSION "AAA")

add_subdirectory(tests)