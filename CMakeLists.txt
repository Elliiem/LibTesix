cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJ TesixTui)
project(${PROJ})

set(SOURCES)
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

foreach(VAL ${SOURCES})
    message(STATUS ${VAL})
endforeach()

set(INCLUDES)
file(GLOB_RECURSE INCLUDES ${CMAKE_SOURCE_DIR}/include/*.h)

foreach(VAL ${INCLUDES})
    message(STATUS ${VAL})
endforeach()

add_library(${PROJ} STATIC
    ${SOURCES}
    ${INCLUDES}
)

target_include_directories(${PROJ} PUBLIC
    ./include
)