#ifndef INC_1_UTILS_H
#define INC_1_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h> 
#include <sys/mman.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <semaphore.h>

#define SHM_NAME1 "/shm_channel1" 
#define SHM_NAME2 "/shm_channel2" 

typedef struct {
    sem_t sem_parent;
    sem_t sem_child;
    char data[256];
} shared_data;

void Create(shared_data *shm);
void SetData(shared_data *shm, char* str);
char* GetData(shared_data *shm);
char* ReadString(FILE* stream);
void Cleanup(shared_data *shm);

#endif