include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
)

FetchContent_MakeAvailable(googletest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CHILD1_PATH "${CMAKE_BINARY_DIR}/lab1/child_1")
set(CHILD2_PATH "${CMAKE_BINARY_DIR}/lab1/child_2")

add_executable(lab1_test 
    lab1_tests.cpp
    ${CMAKE_SOURCE_DIR}/lab1/src/parent.c
    ${CMAKE_SOURCE_DIR}/lab1/src/utils.c
)

add_dependencies(lab1_test child_1 child_2)

target_include_directories(lab1_test PRIVATE 
    ${CMAKE_SOURCE_DIR}/lab1/include
)

target_compile_definitions(lab1_test PRIVATE
    CHILD1_PATH="${CHILD1_PATH}"
    CHILD2_PATH="${CHILD2_PATH}"
)

target_link_libraries(lab1_test PRIVATE 
    gtest
    gtest_main
)

add_test(NAME lab1_test COMMAND lab1_test)