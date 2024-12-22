#ifndef PENDINGOPS_H
#define PENDINGOPS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <zmq.h>
#include <unistd.h>
#include "message.h"
#include "tree.h"

typedef struct PendingOperation {
    int id;
    int parent_id;
    void* socket;
    void* context;
    time_t timestamp;
    char command[32];
    struct PendingOperation* next;
} PendingOperation;

extern PendingOperation* pending_ops;

void add_pending_operation(int id, void* socket, void* context, const char* command);
void add_create_operation(int id, int parent_id, void* socket, void* context, const char* command);
void cleanup_pending_operations(void);
void check_pending_responses(void);

#endif 