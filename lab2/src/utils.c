#include "utils.h"

void Fisher_Yates_algorithm(int* coloda){
    unsigned int seed = (unsigned int)pthread_self();
    for (int i = COLODA_SIZE - 1; i > 0; --i){
        int j = rand_r(&seed)%(i+1);
        int tmp = coloda[j];
        coloda[j] = coloda[i];
        coloda[i] = tmp;
    }
}