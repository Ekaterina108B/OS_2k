add_executable(monte_carlo main.c src/monte_carlo_maps.c src/utils.c)
target_include_directories(monte_carlo PRIVATE include)