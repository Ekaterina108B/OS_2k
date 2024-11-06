#include "monte_carlo_maps.h"
#include "utils.h"

double Process(int count_of_fork, int count_of_raunds){
    int count_of_matches = 0;
    double result;
    int* pipe_fd[2 * count_of_fork];
    pid_t childs[count_of_fork];

    for (int i = 0; i < count_of_fork; ++i){
        CreatePipe(pipe_fd + 2 * i);

        pid_t child = fork();
        childs[i] = child;
        if (child < 0){
            perror("Ошибка fork для первого процесса");
            exit(EXIT_FAILURE);
        } else if (child == 0){
            close(pipe_fd[2*i + READ_END]);
            int raunds = (i < count_of_fork - 1) ? count_of_raunds / count_of_fork : count_of_raunds % count_of_fork;
            int matches = Monte_Carlo_Maps(raunds);

            write(pipe_fd[2*i + WRITE_END], &matches, sizeof(int));
            close(pipe_fd[2*i + WRITE_END]);
            exit(0);
        }
        close(pipe_fd[2*i + WRITE_END]);
    }

    for (int i = 0; i < count_of_fork; ++i){
        int result_child;
        waitpid(childs[i], NULL, 0);
        read(pipe_fd[2*i + READ_END], &result_child, sizeof(int));
        count_of_matches += result_child;
        close(pipe_fd[2*i + READ_END]);
    }
    
    result = (double)count_of_matches / (double)count_of_raunds;
    return result;
}

int Monte_Carlo_Maps(int count_of_raunds) {
    int coloda[COLODA_SIZE];
    for  (int i = 0; i < COLODA_SIZE; ++i){ coloda[i] = i; }
    int count_of_matches = 0;

    for (int i = 0; i < count_of_raunds; ++i){
        Fisher_Yates_algorithm(coloda);
        if ((coloda[0] % RANK) == (coloda[1] % RANK)){ ++count_of_matches; }
    }

    return count_of_matches;
}