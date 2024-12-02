add_library(math1 SHARED src/lib1.c)
target_include_directories(math1 PUBLIC include)

add_library(math2 SHARED src/lib2.c)
target_include_directories(math2 PUBLIC include)


add_executable(prog_1 src/lib1.c src/program_1.c)
target_link_libraries(prog_1 math1)
target_include_directories(prog_1 PRIVATE include)

add_executable(prog_2 src/program_2.c)
target_link_libraries(prog_2 dl)
target_include_directories(prog_2 PRIVATE include)