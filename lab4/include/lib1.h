#ifndef LIB1_H
#define LIB1_H

#define EXPORT
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
EXPORT int GCF(int a, int b);
EXPORT char* translation(long x);
#ifdef __cplusplus
}
#endif

#endif 