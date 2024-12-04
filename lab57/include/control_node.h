#ifndef CONTROLNODE_H
#define CONTROLNODE_H

#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "tree.h"
#include "message.h"
#include "pending_ops.h"
#include "node.h"

int create_compute_node(int id, int parent_id);
int ping_node(int id);
int exec_command(int id, const char* text, const char* pattern);

#endif