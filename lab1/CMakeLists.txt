add_executable(OS_1_inversion include/parent.h include/utils.h src/parent.c src/utils.c src/child_1.c src/child_2.c main.c)

target_include_directories(OS_1_inversion PRIVATE include)