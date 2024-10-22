#include <stdio.h>
#include <stdlib.h>

#include "parent.h"

int main(void) {

    ParentRoutine(getenv("PATH_TO_CHILD_1"), getenv("PATH_TO_CHILD_2"), stdin);

    return 0;
}