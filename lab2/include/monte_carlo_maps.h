#ifndef MONTE_CARLO_H
#define MONTE_CARLO_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

double Process(int count_of_fork, int count_of_raunds);
int Monte_Carlo_Maps(int count_of_raunds);

#endif