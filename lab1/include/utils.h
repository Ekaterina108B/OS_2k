#ifndef INC_1_UTILS_H
#define INC_1_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum PipeEnd {
    READ_END,
    WRITE_END
};

void CreatePipe(int pipe_fd[2]);
char* ReadString(FILE* stream);

#endif