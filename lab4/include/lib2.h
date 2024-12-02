#ifndef LIB2_H
#define LIB2_H

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