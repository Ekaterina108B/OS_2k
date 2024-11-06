#include "monte_carlo_maps.h"

int main(const int count_arg, const int* arg[]){
    if (count_arg != 3) {
        printf("Usage: %s <number_rounds> <number_processes>\n", arg[0]);
        return 1;
    }
    
    double result;
    int count_of_fork = arg[1];
    int count_of_raunds = arg[2];

    if (count_of_raunds < 1) {
        printf("The number of rounds cannot be less than 1.\n");
        exit(EXIT_FAILURE);
    }
    if (count_of_fork < 1) {
        printf("The number of rounds cannot be less than 1.\n");
        exit(EXIT_FAILURE);
    }

    result = Process(count_of_fork, count_of_raunds);
    printf("The probability that there are two identical cards on the top: %.6f\n", result);
    printf("Number of processes: %d\n", count_of_fork);
    printf("Number of rounds: %d\n", count_of_raunds);

    return 0;
}