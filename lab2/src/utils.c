#include "utils.h"

void CreatePipe(int pipe_fd[2]){
    if (pipe(pipe_fd) != 0){
        printf("Couldn't create pipe\n");
        exit(EXIT_FAILURE);
    }
}

void Fisher_Yates_algorithm(int* coloda){
    srand(time(NULL) ^ getpid());

    for (int i = COLODA_SIZE - 1; i > 0; --i){
        int j = rand()%(i+1);
        int tmp = coloda[j];
        coloda[j] = coloda[i];
        coloda[i] = tmp;
    }
}