add_executable(server src/server.c)
add_executable(client src/client.c)

target_include_directories(server PRIVATE include)
target_include_directories(client PRIVATE include)

target_link_libraries(server rt)
target_link_libraries(client rt pthread)

