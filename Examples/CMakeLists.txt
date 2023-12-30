cmake_minimum_required(VERSION 3.25)
project(Examples)

file(GLOB SUBDIRS "${CMAKE_SOURCE_DIR}/Examples/*" LIST_DIRECTORIES TRUE)

foreach(DIR ${SUBDIRS})
    if(NOT IS_DIRECTORY "${DIR}")
        remove(SUBDIRS "${DIR}")
    endif() 
endforeach()

foreach(DIR ${SUBDIRS})
    add_subdirectory("${DIR}")
endforeach()