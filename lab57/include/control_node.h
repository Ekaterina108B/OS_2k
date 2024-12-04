#ifndef CONTROLNODE_H
#define CONTROLNODE_H

#include "node.h"

int create_compute_node(int id, int parent_id);
int ping_node(int id);
int exec_command(int id, const char* text, const char* pattern);

#endif