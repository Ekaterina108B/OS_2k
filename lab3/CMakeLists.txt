find_library(RT_LIB rt REQUIRED)

add_executable(parent main.c src/parent.c src/utils.c)
target_include_directories(parent PRIVATE include)
add_executable(child src/child.c src/utils.c)
target_include_directories(child PRIVATE include)

target_link_libraries(parent ${RT_LIB})
target_link_libraries(child ${RT_LIB})

install(TARGETS parent child
        RUNTIME DESTINATION bin)

add_custom_target(clean_shm
    COMMAND rm -f /dev/shm/shm_channel1
    COMMAND rm -f /dev/shm/shm_channel2
    COMMENT "Cleaning shared memory files"
) 