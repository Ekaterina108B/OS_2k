#include "pending_ops.h"

void add_pending_operation(int id, void* socket, void* context, const char* command) {
    PendingOperation* op = (PendingOperation*)malloc(sizeof(PendingOperation));
    op->id = id;
    op->socket = socket;
    op->context = context;
    op->timestamp = time(NULL);
    strncpy(op->command, command, sizeof(op->command) - 1);
    op->command[sizeof(op->command) - 1] = '\0';
    op->next = pending_ops;
    pending_ops = op;
}

void cleanup_pending_operations(void) {
    PendingOperation* current = pending_ops;
    PendingOperation* prev = NULL;
    time_t now = time(NULL);

    while(current != NULL) {
        if(now - current->timestamp > 5) {
            PendingOperation* to_delete = current;
            if(prev) {
                prev->next = current->next;
            } else {
                pending_ops = current->next;
            }
            zmq_close(to_delete->socket);
            zmq_ctx_destroy(to_delete->context);
            free(to_delete);
            if(prev){ current = prev->next; }
            else {current = pending_ops; }
        } else {
            prev = current;
            current = current->next;
        }
    }
}

void check_pending_responses(void) {
    PendingOperation* current = pending_ops;
    PendingOperation* prev = NULL;

    while(current != NULL) {
        zmq_pollitem_t items[1];
        items[0].socket = current->socket;
        items[0].events = ZMQ_POLLIN;

        int poll_result = zmq_poll(items, 1, 0);
        if(poll_result == 1) {
            Message msg = {0};
            int recv_result = receive_message(current->socket, &msg);
            if(recv_result > 0) {
                if(strcmp(current->command, CMD_PING) == 0) {
                    printf("Ok: 1\n");
                } else {
                    printf("Ok:%d: %s\n", current->id, msg.data);
                }
                if(prev) { prev->next = current->next; }
                else { pending_ops = current->next; }
                zmq_close(current->socket);
                zmq_ctx_destroy(current->context);
                free(current);
                if(prev){ current = prev->next; }
                else { current = pending_ops; }
                printf("> ");
                fflush(stdout);
                continue;
            }
        }
        
        time_t now = time(NULL);
        if(now - current->timestamp >= 1) {
            if(strcmp(current->command, CMD_PING) == 0) {
                printf("Ok: 0\n");
            } else if(strcmp(current->command, CMD_EXEC) == 0) {
                printf("Error:%d: Node is unavailable\n", current->id);
            }
            if(prev) { prev->next = current->next; }
            else { pending_ops = current->next; }
            zmq_close(current->socket);
            zmq_ctx_destroy(current->context);
            PendingOperation* to_delete = current;
            if(prev){ current = prev->next; }
            else { current = pending_ops; }
            free(to_delete);
            printf("> ");
            fflush(stdout);
            continue;
        }
        
        prev = current;
        current = current->next;
    }
} 