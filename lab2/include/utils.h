#ifndef UTILS_H
#define UTILS_H

#include <time.h>

#define COLODA_SIZE 52
#define RANK 13

enum PipeEnd {
    READ_END,
    WRITE_END
};

void CreatePipe(int pipe_fd[2]);
void Fisher_Yates_algorithm(int coloda[]);

#endif