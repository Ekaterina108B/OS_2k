#include "control_node.h"

int create_compute_node(int id, int parent_id) {
    if(is_node_exists(root, id)) {
        printf("Error: Already exists\n");
        return -1;
    }

    if(root == NULL){
        pid_t pid = fork();
        if(pid == 0){
            char id_str[16], parent_id_str[16];
            sprintf(id_str, "%d", id);
            sprintf(parent_id_str, "-1");
            execl("./bin/compute_node", "compute_node", id_str, parent_id_str, NULL);
            perror("Error starting compute node");
            exit(1);
        }
        
        char endpoint[64];
        sprintf(endpoint, "tcp://localhost:%d", 5555 + id);
        root = insert_node(root, id, pid, endpoint, -1);
        printf("Ok: %d\n", pid);
        return pid;
    }

    if(parent_id != -1) {
        TreeNode* current = root;
        
        while(current != NULL){
            if(id < current->id){
                current = current->left;
            } else {
                current = current->right;
            }
        }
        parent_id = current->parent->id;
    } else {
        if (!is_node_exists(root, parent_id)) {
            printf("Error: Parent not found\n");
            return -1;
        }
    }

    void* context = zmq_ctx_new();
    void* socket = zmq_socket(context, ZMQ_REQ);
    int timeout = 1000;
    int linger = 0;
    zmq_setsockopt(socket, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    zmq_setsockopt(socket, ZMQ_SNDTIMEO, &timeout, sizeof(timeout));
    zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(linger));
    if(zmq_connect(socket, root->endpoint) != 0){
        zmq_close(socket);
        zmq_ctx_destroy(context);
        printf("Error: Root is unavailable\n");
        return -1;
    }

    Message msg = {0};
    strcpy(msg.command, CMD_CREATE);
    msg.source_id = 0;
    msg.target_id = parent_id;
    msg.create_child_id = id;
    if(send_message(socket, &msg) <= 0){
        zmq_close(socket);
        zmq_ctx_destroy(context);
        printf("Error: Root is unavailable\n");
        return -1;
    }

    add_pending_operation(id, socket, context, CMD_CREATE);
    return 0;
}

int ping_node(int id) {
    if(root == NULL){
        printf("Error: Not found\n");
        return -1;
    }

    if(!is_node_exists(root, id)){
        printf("Error: Node not found\n");
        return -1;
    }

    void* context = zmq_ctx_new();
    void* socket = zmq_socket(context, ZMQ_REQ);
    int timeout = 1000;
    int linger = 0;
    zmq_setsockopt(socket, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    zmq_setsockopt(socket, ZMQ_SNDTIMEO, &timeout, sizeof(timeout));
    zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(linger));
    if(zmq_connect(socket, root->endpoint) != 0){
        zmq_close(socket);
        zmq_ctx_destroy(context);
        printf("Ok: 0\n");
        return 0;
    }

    Message msg = {0};
    strcpy(msg.command, CMD_PING);
    msg.source_id = 0;
    msg.target_id = id;
    if(send_message(socket, &msg) <= 0){
        zmq_close(socket);
        zmq_ctx_destroy(context);
        printf("Ok: 0\n");
        return 0;
    }

    add_pending_operation(id, socket, context, CMD_PING);
    return 0;
}

int exec_command(int id, const char* text, const char* pattern) {
    if(root == NULL){
        printf("Error:%d: Not found\n", id);
        return -1;
    }

    if(!is_node_exists(root, id)){
        printf("Error:%d: Node not found\n", id);
        return -1;
    }

    void* context = zmq_ctx_new();
    void* socket = zmq_socket(context, ZMQ_REQ);
    int timeout = 1000;
    int linger = 0;
    zmq_setsockopt(socket, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    zmq_setsockopt(socket, ZMQ_SNDTIMEO, &timeout, sizeof(timeout));
    zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(linger));
    if(zmq_connect(socket, root->endpoint) != 0){
        zmq_close(socket);
        zmq_ctx_destroy(context);
        printf("Error:%d: Node is unavailable\n", id);
        return -1;
    }

    Message msg = {0};
    strcpy(msg.command, CMD_EXEC);
    msg.source_id = 0;
    msg.target_id = id;
    strcpy(msg.data, text);
    strcpy(msg.data + strlen(text) + 1, pattern);
    if (send_message(socket, &msg) <= 0) {
        zmq_close(socket);
        zmq_ctx_destroy(context);
        printf("Error:%d: Node is unavailable\n", id);
        return -1;
    }
    
    add_pending_operation(id, socket, context, CMD_EXEC);
    return 0;
} 

int show_tree(void){
    if(root == NULL){
        printf("Error: Tree is empty\n");
        return -1;
    }
    print_tree(root);
    return 0;
} 