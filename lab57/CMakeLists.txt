include_directories(include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb -Wall -Wextra")

find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)

set(SRC_DIR src)
set(BIN_DIR bin)

set(CONTROL_SOURCES
    ./main.c
    ${SRC_DIR}/control_node.c
    ${SRC_DIR}/message.c
    ${SRC_DIR}/tree.c
    ${SRC_DIR}/pending_ops.c
)

set(COMPUTE_SOURCES
    ${SRC_DIR}/compute_node.c
    ${SRC_DIR}/message.c
)

add_executable(control_node ${CONTROL_SOURCES})
target_link_libraries(control_node ${ZMQ_LIBRARIES})
add_executable(compute_node ${COMPUTE_SOURCES})
target_link_libraries(compute_node ${ZMQ_LIBRARIES})

set_target_properties(control_node PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${BIN_DIR})
set_target_properties(compute_node PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${BIN_DIR})