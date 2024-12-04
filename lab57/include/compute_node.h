#ifndef COMPUTENODE_H
#define COMPUTENODE_H

#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "message.h"
#include "node.h"

void start_compute_node(int id, int parent_id);
char* find_substring(const char* text, const char* pattern);

#endif