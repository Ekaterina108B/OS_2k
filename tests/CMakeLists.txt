include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
)

FetchContent_MakeAvailable(googletest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${ZMQ_INCLUDE_DIRS})

enable_testing()

add_library(common_lib
    ../lab57/src/message.c
    ../lab57/src/tree.c
)

target_include_directories(common_lib PUBLIC ../lab57/include)

add_executable(test_tree lab57_tree.cpp)
target_link_libraries(test_tree 
    gtest 
    gtest_main 
    pthread 
    zmq
    common_lib
)

add_executable(test_message lab57_message.cpp)
target_link_libraries(test_message 
    gtest 
    gtest_main 
    pthread 
    zmq
    common_lib
)

add_test(NAME TreeTest COMMAND test_tree)
add_test(NAME MessageTest COMMAND test_message)