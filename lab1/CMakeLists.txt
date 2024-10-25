add_executable(parent main.c src/parent.c src/utils.c)
target_include_directories(parent PRIVATE include)

add_executable(child_1 src/child_1.c src/utils.c)
target_include_directories(child_1 PRIVATE include)

add_executable(child_2 src/child_2.c src/utils.c)
target_include_directories(child_2 PRIVATE include)