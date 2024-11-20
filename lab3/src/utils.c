#include "utils.h"

void Create(shared_data *shm){
    memset(shm->data, 0, sizeof(shm->data));
    if (sem_init(&shm->sem_parent, 1, 0) == -1) {
        perror("Failed to initialize parent semaphore");
        exit(EXIT_FAILURE);
    }
    if (sem_init(&shm->sem_child, 1, 0) == -1) {
        perror("Failed to initialize child semaphore");
        exit(EXIT_FAILURE);
    }
}

void SetData(shared_data *shm, char* str){
    strncpy(shm->data, str, sizeof(shm->data) - 1);
    shm->data[sizeof(shm->data) - 1] = '\0';
}

char* GetData(shared_data *shm){
    return shm->data;
}

char* ReadString(FILE* stream){
    if (feof(stream)){
        return NULL;
    }

    const int MAX_SIZE = 256;
    char* buffer = (char*)malloc(MAX_SIZE);
    int buffer_size = MAX_SIZE;

    if(!buffer) {
        printf("Couldn't allocate buffer");
        exit(EXIT_FAILURE);
    }

    char read;
    int index = 0;
    while ((read = fgetc(stream)) != EOF){
        if (read == '\0'){
            free(buffer);
            return NULL;
        }
        
        buffer[index++] = read;
        if (read == '\n'){
            break;
        }
        
        if(index == buffer_size){
            buffer = realloc(buffer, buffer_size + MAX_SIZE);
            buffer_size += MAX_SIZE;
        }
    }
    
    if (index == 0){
        free(buffer);
        return NULL;
    }
    
    if (buffer[index-1] == '\n'){
        buffer[index-1] = '\0';
    } else {
        buffer[index] = '\0';
    }
    
    return buffer;
}

void Cleanup(shared_data *shm) {
    sem_destroy(&shm->sem_parent);
    sem_destroy(&shm->sem_child);
}