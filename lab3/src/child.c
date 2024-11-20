#include <ctype.h>

#include "utils.h"

int main(const int count_arg, const char* arg[]){
    if(count_arg != 2){
        printf("Necessary arguments were not provided\n");
        exit(EXIT_FAILURE);
    }
    
    int shm_fd = shm_open(arg[1], O_RDWR, 0666);
    if (shm_fd == -1){
        perror("Failed to open shared_memory\n");
        exit(EXIT_FAILURE);
    }
    shared_data *shm = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm == MAP_FAILED){
        perror("Failed Failed to map to the address space (mmap)\n");
        exit(EXIT_FAILURE);
    }

    int input_len;
    char copy;
    while (1){
        sem_wait(&shm->sem_child);

        if (strcmp(shm->data, "EXIT") == 0){ break; }
        
        char input[256];
        strncpy(input, shm->data, sizeof(input) - 1);
        input[sizeof(input) - 1] = '\0';
        input_len = strlen(input);
        for (int i=0; i<input_len/2 + 1; ++i){
            copy = input[i];
            input[i] = input[input_len-i-1];
            input[input_len-i-1] = copy;
        }

        printf("%s\n", input);
        fflush(stdout);
        sem_post(&shm->sem_parent);
    }

    Cleanup(shm);
    munmap(shm, sizeof(shared_data));
    close(shm_fd);

    return 0;
}