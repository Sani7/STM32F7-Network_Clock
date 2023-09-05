# Third Party
include_directories(${GTEST_INCLUDE_DIR})

link_directories(${GTEST_LIB_DIR})

find_package(GTest REQUIRED)

# tests
file(GLOB_RECURSE TEST_SOURCES "*.cpp")
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests ${LIBS})

target_compile_options(tests PRIVATE $<$<CONFIG:Debug>:
    -Wall -Wextra -pedantic-errors -Wconversion -Wsign-conversion
    >)
target_link_libraries(tests
    PRIVATE
    gtest
    GTest::gtest_main
)

target_include_directories(tests
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${PROJECT_BINARY_DIR}
)

include(GoogleTest)
include (CTest)
gtest_discover_tests(tests)