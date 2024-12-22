#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "compute_node.h"
#include "control_node.h"
#include "tree.h"
#include "message.h"
#include "pending_ops.h"

#define MAX_CMD_LENGTH 1024

TreeNode* root = NULL;
PendingOperation* pending_ops = NULL;

void handle_signal(int sig) {
    (void)sig;
    if(root != NULL){
        kill_tree(root);
        free_tree(root);
    }
    exit(0);
}

void check_pending_responses(void);
void cleanup_pending_operations(void);

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    char command[MAX_CMD_LENGTH];
    char text[MAX_TEXT_LENGTH];
    char pattern[MAX_TEXT_LENGTH];
    int id, parent_id;
    pid_t pid;

    zmq_pollitem_t items[1];
    items[0].socket = NULL;
    items[0].fd = STDIN_FILENO;
    items[0].events = ZMQ_POLLIN;

    printf("> ");
    fflush(stdout);

    while(1){
        int rc = zmq_poll(items, 1, 100);
        cleanup_pending_operations();
        check_pending_responses();
        if(rc == 0){ continue; }
        if(items[0].revents & ZMQ_POLLIN){
            if(fgets(command, MAX_CMD_LENGTH, stdin) == NULL) {
                break;
            }
            command[strcspn(command, "\n")] = 0;

            if(sscanf(command, "create %d %d", &id, &parent_id) == 2){
                create_compute_node(id, parent_id);
            } else if(sscanf(command, "create %d", &id) == 1){
                create_compute_node(id, -1);
            } else if(sscanf(command, "ping %d", &id) == 1){
                ping_node(id);
            } else if(sscanf(command, "exec %d", &id) == 1){
                printf("> ");
                if (fgets(text, MAX_TEXT_LENGTH, stdin) == NULL) {
                    break;
                }
                text[strcspn(text, "\n")] = 0;

                printf("> ");
                if (fgets(pattern, MAX_TEXT_LENGTH, stdin) == NULL) {
                    break;
                }
                pattern[strcspn(pattern, "\n")] = 0;

                exec_command(id, text, pattern);
            } else if(sscanf(command, "kill %d", &pid) == 1){
                if(kill(pid, SIGTERM) == 0){
                    printf("Ok: Process %d killed\n", pid);
                } else {
                    printf("Error: Failed to kill process %d\n", pid);
                }
            } else if(strcmp(command, "exit") == 0){
                break;
            } else if(strcmp(command, "tree") == 0){
                show_tree();
            } else {
                printf("Error: Unknown command\n");
            }
            printf("> ");
            fflush(stdout);
        }
    }
    
    return 0;
} 