#include "lib1.h"
#include <stdio.h>

int main() {
    int command;
    
    while(1) {
        scanf("%d", command);

        if(command == 1){
            int a, b, result;
            scanf("%d %d", &a, &b);
            result = GCF(a, b);
            printf("GCF: %d\n", result);

        } else if(command == 2){
            long x;
            char* result;
            scanf("%ld", &x);
            result = translation(x);
            printf("In the 2NS is %s\n", result);
            free(result);
            
        } else {
            printf("Invalid command, enter again\n");
        }
    }
    
    return 0;
} 