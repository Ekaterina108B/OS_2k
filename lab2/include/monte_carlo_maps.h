#ifndef MONTE_CARLO_H
#define MONTE_CARLO_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int raunds;
    int matches;
} ThreadData;

double Process(int count_of_thread, int count_of_raunds);
void* Monte_Carlo_Maps(void *arg);

#endif