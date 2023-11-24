cmake_minimum_required(VERSION 3.5)

project(bouncing_box)

add_executable(${PROJECT_NAME}
    bouncing_box.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    libTesix
)
