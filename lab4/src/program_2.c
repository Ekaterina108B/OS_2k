#include <stdio.h>
#include <dlfcn.h>
#include <stdbool.h>
#include <stdlib.h>

typedef int (*GCF_func)(int, int);
typedef char* (*translation_func)(long);

int main() {
    int command;
    bool flag_use_lib1 = false;

    void* lib = dlopen("./libmath1.so", RTLD_LAZY);
    if (!lib) {
        fprintf(stderr, "Cannot open library: %s\n", dlerror());
        return 1;
    }
    GCF_func GCF = (GCF_func)dlsym(lib, "GCF");
    translation_func translation = (translation_func)dlsym(lib, "translation");
    if (!GCF || !translation) {
        fprintf(stderr, "Cannot load symbols: %s\n", dlerror());
        dlclose(lib);
        return 1;
    }
    flag_use_lib1 = true;
    printf("Library 1 is loaded by default.\n\n");

    while(1){
        scanf("%d", &command);

        if(command == 0){
            dlclose(lib);
            if(flag_use_lib1){
                lib = dlopen("./libmath2.so", RTLD_LAZY);
                if (!lib) {
                    fprintf(stderr, "Cannot open library: %s\n", dlerror());
                    return 1;
                }
                printf("Library 2 is loaded.\n");
            } else {
                lib = dlopen("./libmath1.so", RTLD_LAZY);
                if (!lib) {
                    fprintf(stderr, "Cannot open library: %s\n", dlerror());
                    return 1;
                }
                printf("Library 1 is loaded.\n");
            }
            flag_use_lib1 = !flag_use_lib1;
            GCF = (GCF_func)dlsym(lib, "GCF");
            translation = (translation_func)dlsym(lib, "translation");
            if (!GCF || !translation) {
                fprintf(stderr, "Cannot load symbols: %s\n", dlerror());
                dlclose(lib);
                return 1;
            }

        } else if(command == 1){
            int a, b, result;
            scanf("%d %d", &a, &b);
            result = GCF(a, b);
            printf("GCF: %d\n", result);

        } else if(command == 2){
            long x;
            char* result;
            scanf("%ld", &x);
            result = translation(x);
            if(flag_use_lib1) { printf("In the 2NS is %s\n", result); }
            else { printf("In the 3NS is %s\n", result); }
            free(result);

        } else {
            printf("Invalid command, enter again\n");
        }
    }
    dlclose(lib);

    return 0;
}