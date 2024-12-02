#include "lib1.h"
#include <cstring>
#include <cstdlib>

int GCF(int a, int b) {
    if (a < 1 || b < 1){
        printf("Numbers must be natural.\n");
        return 0;
    }
    
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

char* translation(long x) {
    if (x < 0) {
        printf("Numbers must be natural or 0.\n");
        return 0;
    }
    
    char* result = new char[65];
    int i = 0;
    
    if (x == 0) {
        result[0] = '0';
        result[1] = '\0';
        return result;
    }
    
    while (x > 0) {
        result[i++] = (x % 2) + '0';
        x /= 2;
    }
    result[i] = '\0';
    
    for (int j=0; j<i/2+1; ++j) {
        char temp = result[j];
        result[j] = result[i-1-j];
        result[i-1-j] = temp;
    }
    return result;
} 