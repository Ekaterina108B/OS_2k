#include "utils.h"

void Fisher_Yates_algorithm(int* coloda){

    for (int i = COLODA_SIZE - 1; i > 0; --i){
        int j = rand()%(i+1);
        int tmp = coloda[j];
        coloda[j] = coloda[i];
        coloda[i] = tmp;
    }
}