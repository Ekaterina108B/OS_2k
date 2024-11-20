#include "parent.h"
#include "utils.h"

void ParentRoutine(const char* path_to_child, FILE* stream) {
    char file_name_1[256], file_name_2[256];
    fscanf(stream, "%s\n", file_name_1);
    fscanf(stream, "%s\n", file_name_2);

    int file_1 = open(file_name_1, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_1 < 0){
        perror("Failed to open file\n");
        exit(EXIT_FAILURE);
    }
    int file_2 = open(file_name_2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_2 < 0){
        perror("Failed to open file\n");
        close(file_1);
        exit(EXIT_FAILURE);
    }

    int fd_1 = shm_open(SHM_NAME1, O_CREAT | O_RDWR, 0666);
    int fd_2 = shm_open(SHM_NAME2, O_CREAT | O_RDWR, 0666);
    if (fd_1 == -1 || fd_2 == -1){
        perror("Failed to open shared_memory\n");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd_1, sizeof(shared_data)) == -1 || ftruncate(fd_2, sizeof(shared_data)) == -1){
        perror("Failed to set size ftruncate\n");
        exit(EXIT_FAILURE);
    }
    shared_data *shm_1 = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, fd_1, 0);
    shared_data *shm_2 = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, fd_2, 0);
    if (shm_1 == MAP_FAILED || shm_2 == MAP_FAILED){
        perror("Failed Failed to map to the address space (mmap)\n");
        exit(EXIT_FAILURE);
    }

    Create(shm_1);
    Create(shm_2);

    pid_t child_1 = fork();
    if (child_1 < 0){
        perror("Ошибка fork для первого процесса");
        exit(EXIT_FAILURE);
    } else if (child_1 == 0){
        close(file_2);
        dup2(file_1, STDOUT_FILENO);
        close(file_1);
        
        char* arg[3];
        arg[0] = "child";
        arg[1] = SHM_NAME1;
        arg[2] = NULL;
        
        if(execv(path_to_child, arg) == -1){
            perror("Failed to exec\n");
            exit(EXIT_FAILURE);
        }
    }

    pid_t child_2 = fork();
    if (child_2 < 0){
        perror("Ошибка fork для второго процесса");
        exit(EXIT_FAILURE);
    } else if (child_2 == 0){
        close(file_1);
        dup2(file_2, STDOUT_FILENO);
        close(file_2);

        char* arg[3];
        arg[0] = "child";
        arg[1] = SHM_NAME2;
        arg[2] = NULL;
        
        if(execv(path_to_child, arg) == -1){
            perror("Failed to exec\n");
            exit(EXIT_FAILURE);
        }
    }

    char* input = NULL;
    int chance;
    srand(time(NULL));
    while((input = ReadString(stream)) != NULL) {
        chance = (rand() % 5) + 1;
        if (chance < 5){
            SetData(shm_1, input);
            sem_post(&shm_1->sem_child);
            sem_wait(&shm_1->sem_parent);
        } else {
            SetData(shm_2, input);
            sem_post(&shm_2->sem_child);
            sem_wait(&shm_2->sem_parent);
        }
        free(input);
    }
    
    SetData(shm_1, "EXIT");
    sem_post(&shm_1->sem_child);
    SetData(shm_2, "EXIT");
    sem_post(&shm_2->sem_child);

    waitpid(child_1, NULL, 0);
    waitpid(child_2, NULL, 0);

    Cleanup(shm_1);
    Cleanup(shm_2);
    
    close(file_1);
    close(file_2);

    munmap(shm_1, sizeof(shared_data));
    munmap(shm_2, sizeof(shared_data));
    close(fd_1);
    close(fd_2);
    shm_unlink(SHM_NAME1);
    shm_unlink(SHM_NAME2);
    
}