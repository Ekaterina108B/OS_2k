#include "monte_carlo_maps.h"
#include "utils.h"

double Process(int count_of_thread, int count_of_raunds){
    int count_of_matches = 0;
    double result;
    pthread_t threads[count_of_thread];
    ThreadData thread_data[count_of_thread];

    for (int i = 0; i < count_of_thread; ++i){
        thread_data[i].raunds = (i < count_of_thread - 1) ? count_of_raunds / count_of_thread : (count_of_raunds / count_of_thread) + (count_of_raunds % count_of_thread);
        thread_data[i].matches = 0;

        if (pthread_create(&threads[i], NULL, Monte_Carlo_Maps, (void*)&thread_data[i]) != 0){
            perror("Ошибка при создании потока");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < count_of_thread; ++i){
        pthread_join(threads[i], NULL);
        count_of_matches += thread_data[i].matches;
    }
  
    result = (double)count_of_matches / (double)count_of_raunds;
    printf("Total matches: %d of %d rounds.\n", count_of_matches, count_of_raunds);
    return result;
}

void* Monte_Carlo_Maps(void *arg){
    ThreadData *data = (ThreadData *)arg;
    int local_count = 0;
    
    int coloda[COLODA_SIZE];
    for  (int i = 0; i < COLODA_SIZE; ++i){ coloda[i] = i; }

    for (int i = 0; i < data->raunds; ++i){
        Fisher_Yates_algorithm(coloda);
        if ((coloda[0] % RANK) == (coloda[1] % RANK)){ ++local_count; }
    }
    
    data->matches = local_count;

    return NULL;
}