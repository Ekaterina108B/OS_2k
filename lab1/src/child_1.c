#include <ctype.h>

#include "utils.h"

int main(const int count_arg, const char* arg[]){
    if(count_arg != 2){
        printf("Necessary arguments were not provided\n");
        exit(EXIT_FAILURE);
    }
    FILE* file = fopen(arg[1], "w");
    if (!file){
        printf("Failed to open file\n");
        exit(EXIT_FAILURE);
    }

    char* input = NULL;
    int input_len;
    char copy;

    while ((input = ReadString(stdin)) != NULL){
        input_len = strlen(input);

        for (int i=0; i<input_len/2 + 1; ++i){
            copy = input[i];
            input[i] = input[input_len-i-1];
            input[input_len-i-1] = copy;
        }

        fprintf(file, "%s\n", input);
        free(input);
    }

    fclose(file);
    return 0;
}