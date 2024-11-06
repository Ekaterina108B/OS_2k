#include "monte_carlo_maps.h"
#include "utils.h"

int main(const int count_arg, const char* arg[]){
    if (count_arg != 3) {
        printf("Usage: %s <number_threads> <number_rounds>\n", arg[0]);
        return 1;
    }
    
    double result, time_spent;
    int count_of_thread = atoi(arg[1]);
    int count_of_raunds = atoi(arg[2]);

    if (count_of_raunds < 1) {
        printf("The number of rounds cannot be less than 1.\n");
        return 1;
    }
    if (count_of_thread < 1) {
        printf("The number of threads cannot be less than 1.\n");
        return 1;
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    result = Process(count_of_thread, count_of_raunds);
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("The probability that there are two identical cards on the top: %.6f\n", result);
    printf("Number of threads: %d\n", count_of_thread);
    printf("Number of rounds: %d\n", count_of_raunds);
    printf("Execution time: %f seconds\n", time_spent);

    return 0;
}