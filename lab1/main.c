#include <stdio.h>
#include <stdlib.h>

#include "parent.h"

int main(void) {
    srand(12345);
    ParentRoutine("./lab1/child_1", "./lab1/child_2", stdin);

    return 0;
}