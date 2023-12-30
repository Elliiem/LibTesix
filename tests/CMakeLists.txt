
file(GLOB TEST_SOURCES_G "${CMAKE_SOURCE_DIR}/tests/*.cpp")

set(SOURCE_NAMES)

foreach(SOURCE ${TEST_SOURCES_G})
    get_filename_component(SOURCE_NAME "${SOURCE}" NAME)
    message(STATUS ${SOURCE_NAME})
    list(APPEND SOURCE_NAMES "${SOURCE_NAME}")
endforeach()

set(TEST_SOURCES "${SOURCE_NAMES}")

create_test_sourcelist(Tests
    ${TEST_SOURCES}
)

add_executable(CommonCxxTests ${Tests})

add_test(NAME test COMMAND CommonCxxTests test)
set_property(TEST test PROPERTY PASS_REGULAR_EXPRESSION "\n")