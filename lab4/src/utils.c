#include "utils.h"

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