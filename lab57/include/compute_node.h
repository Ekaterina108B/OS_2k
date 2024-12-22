#ifndef COMPUTENODE_H
#define COMPUTENODE_H

#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "message.h"
#include "node.h"

void* create_socket(int timeout);
void connect_to_child(void* socket, int child_id);
void* setup_child_connection(int child_id);

bool try_child_socket(void** socket, Message* msg, const char* side);
char* find_substring(const char* text, const char* pattern);
void handle_message(Message* msg);
void start_compute_node(int id, int parent_id);
int check_parent(void* context, int parent_id, int id);

#endif