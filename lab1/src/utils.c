#include "utils.h"

void CreatePipe(int pipe_fd[2]){
    if (pipe(pipe_fd) != 0){
        printf("Couldn't create pipe\n");
        exit(EXIT_FAILURE);
    }
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
    while ((read = getc(stream)) != EOF){
        buffer[index++] = read;
        if(index == buffer_size){
            buffer = realloc(buffer, buffer_size + MAX_SIZE);
            buffer_size += MAX_SIZE;
        }
        if (read == '\n'){
            break;
        }
    }
    buffer[index] = '\0';

    return buffer;
}