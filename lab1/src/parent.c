#include <sys/wait.h>
#include <time.h>

#include "parent.h"
#include "utils.h"

void ParentRoutine(const char* path_to_child_1, const char* path_to_child_2, FILE* stream) {
    char file_name_1[256], file_name_2[256];
    fscanf(stream, "%s\n", file_name_1);
    fscanf(stream, "%s\n", file_name_2);
    
    int pipe_fd_1[2];
    CreatePipe(pipe_fd_1);
    int pipe_fd_2[2];
    CreatePipe(pipe_fd_2);

    pid_t child_1 = fork();
    if (child_1 < 0){
        perror("Ошибка fork для первого процесса");
    } else if (child_1 == 0){
        close(pipe_fd_2[READ_END]);
        close(pipe_fd_2[WRITE_END]);
        close(pipe_fd_1[WRITE_END]);
        dup2(pipe_fd_1[READ_END], 0);

        char* arg[3];
        arg[0] = "child_1";
        arg[1] = file_name_1;
        arg[2] = NULL;
        
        if(execv(path_to_child_1, arg) == -1){
            printf("Failed to exec\n");
        }
    }

    pid_t child_2 = fork();
    if (child_2 < 0){
        perror("Ошибка fork для второго процесса");
    } else if (child_2 == 0){
        close(pipe_fd_1[READ_END]);
        close(pipe_fd_1[WRITE_END]);
        close(pipe_fd_2[WRITE_END]);
        dup2(pipe_fd_2[READ_END], 0);

        char* arg[3];
        arg[0] = "child_2";
        arg[1] = file_name_2;
        arg[2] = NULL;
        
        if(execv(path_to_child_2, arg) == -1){
            printf("Failed to exec\n");
        }
    }

    close(pipe_fd_1[READ_END]);
    close(pipe_fd_2[READ_END]);
    char* input = NULL;
    int chance;
    while((input = ReadString(stream)) != NULL) {
        chance = (rand() % 5) + 1;
        if (chance < 5){
            //передаю первому ребёнку
            write(pipe_fd_1[WRITE_END], input, strlen(input));
            write(pipe_fd_1[WRITE_END], "\n", 1);
        } else {
            //передаю второму ребёнку
            write(pipe_fd_2[WRITE_END], input, strlen(input));
            write(pipe_fd_2[WRITE_END], "\n", 1);
        }
        free(input);
    }
    write(pipe_fd_1[WRITE_END], "\0", 1);
    write(pipe_fd_2[WRITE_END], "\0", 1);

    close(pipe_fd_1[WRITE_END]);
    close(pipe_fd_2[WRITE_END]);
    waitpid(child_1, NULL, 0);
    waitpid(child_2, NULL, 0);
    
}