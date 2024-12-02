include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
)

FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)


set(CMAKE_CXX_STANDARD 20)

add_executable(lab4_test_lib1 lab4_test_lib1.cpp)
target_link_libraries(lab4_test_lib1 PRIVATE GTest::GTest)
target_link_libraries(test_lib1 math1)
target_include_directories(lab4_test_lib1 PRIVATE ../lab4/include)
add_test(lab4_test_lib1 lab4_test_lib1)

add_executable(lab4_test_lib2 lab4_test_lib2.cpp)
target_link_libraries(lab4_test_lib2 PRIVATE GTest::GTest)
target_link_libraries(test_lib2 math2)
target_include_directories(lab4_test_lib2 PRIVATE ../lab4/include)
add_test(lab4_test_lib2 lab4_test_lib2)