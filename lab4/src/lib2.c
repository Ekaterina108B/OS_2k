#include "lib2.h"
#include <stdlib.h>

int GCF(int a, int b) {
    if(a < 0 || b < 0){
        printf("Numbers must be natural ir 0.\n");
        return 0;
    }
    
    int divider = 1;
    for(int i=1; (i<=a && i<=b); ++i) {
        if((a % i == 0) && (b % i == 0)) {
            divider = i;
        }
    }
    return divider;
}

char* translation(long x) {
    if(x < 0) {
        printf("Numbers must be natural or 0.\n");
        return 0;
    }
    
    char* result = (char*)malloc(65 * sizeof(char));
    int i = 0;
    
    if(x == 0) {
        result[0] = '0';
        result[1] = '\0';
        return result;
    }
    
    while(x > 0) {
        result[i++] = (x % 3) + '0';
        x /= 3;
    }
    result[i] = '\0';
    
    for(int j=0; j<i/2+1; ++j) {
        char temp = result[j];
        result[j] = result[i-1-j];
        result[i-1-j] = temp;
    }
    
    return result;
} 