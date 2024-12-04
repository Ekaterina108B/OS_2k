#include "control_node.h"

int create_compute_node(int id, int parent_id) {
    if(is_node_exists(root, id)) {
        printf("Error: Already exists\n");
        return -1;
    }
    if(parent_id != -1) {
        TreeNode* parent = find_node(root, parent_id);
        if(parent == NULL) {
            printf("Error: Parent not found\n");
            return -1;
        }

        void* context = zmq_ctx_new();
        void* socket = zmq_socket(context, ZMQ_REQ);
        int timeout = 1000;
        int linger = 0;
        zmq_setsockopt(socket, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
        zmq_setsockopt(socket, ZMQ_SNDTIMEO, &timeout, sizeof(timeout));
        zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(linger));
        if(zmq_connect(socket, parent->endpoint) != 0) {
            zmq_close(socket);
            zmq_ctx_destroy(context);
            printf("Error: Parent is unavailable\n");
            return -1;
        }
        Message msg = {0};
        strcpy(msg.command, CMD_PING);
        msg.source_id = 0;
        msg.target_id = parent_id;
        if(send_message(socket, &msg) <= 0) {
            zmq_close(socket);
            zmq_ctx_destroy(context);
            printf("Error: Parent is unavailable\n");
            return -1;
        }

        zmq_pollitem_t items[1];
        items[0].socket = socket;
        items[0].events = ZMQ_POLLIN;
        if(zmq_poll(items, 1, 1000) <= 0) {
            zmq_close(socket);
            zmq_ctx_destroy(context);
            printf("Error: Parent is unavailable\n");
            return -1;
        }

        if(receive_message(socket, &msg) <= 0) {
            zmq_close(socket);
            zmq_ctx_destroy(context);
            printf("Error: Parent is unavailable\n");
            return -1;
        }
        zmq_close(socket);
        zmq_ctx_destroy(context);
    }

    pid_t pid = fork();
    if(pid == 0) {
        char id_str[16], parent_id_str[16];
        sprintf(id_str, "%d", id);
        sprintf(parent_id_str, "%d", parent_id);
        char executable_path[256];
        sprintf(executable_path, "%s/compute_node", "./bin");
        execl(executable_path, "compute_node", id_str, parent_id_str, NULL);
        perror("Error starting compute node");
        exit(1);
    } else if(pid > 0) {
        char endpoint[64];
        sprintf(endpoint, "tcp://localhost:%d", 5555 + id);
        root = insert_node(root, id, pid, endpoint);
        printf("Ok: %d\n", pid);
        return pid;
    } else {
        printf("Error: Failed to create process\n");
        return -1;
    }
}

int ping_node(int id) {
    TreeNode* node = find_node(root, id);
    if(!node) {
        printf("Error: Not found\n");
        return -1;
    }
    void* context = zmq_ctx_new();
    void* socket = zmq_socket(context, ZMQ_REQ);
    int timeout = 1000;
    int linger = 0;
    zmq_setsockopt(socket, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    zmq_setsockopt(socket, ZMQ_SNDTIMEO, &timeout, sizeof(timeout));
    zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(linger));
    if(zmq_connect(socket, node->endpoint) != 0) {
        zmq_close(socket);
        zmq_ctx_destroy(context);
        printf("Ok: 0\n");
        return 0;
    }

    Message msg = {0};
    strcpy(msg.command, CMD_PING);
    msg.source_id = 0;
    msg.target_id = id;
    if(send_message(socket, &msg) <= 0) {
        zmq_close(socket);
        zmq_ctx_destroy(context);
        printf("Ok: 0\n");
        return 0;
    }

    add_pending_operation(id, socket, context, CMD_PING);
    return 0;
}

int exec_command(int id, const char* text, const char* pattern) {
    TreeNode* node = find_node(root, id);
    if(!node) {
        printf("Error:%d: Not found\n", id);
        return -1;
    }
    void* context = zmq_ctx_new();
    void* socket = zmq_socket(context, ZMQ_REQ);
    int timeout = 1000;
    int linger = 0;
    zmq_setsockopt(socket, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    zmq_setsockopt(socket, ZMQ_SNDTIMEO, &timeout, sizeof(timeout));
    zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(linger));
    if(zmq_connect(socket, node->endpoint) != 0) {
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
    if(send_message(socket, &msg) <= 0) {
        zmq_close(socket);
        zmq_ctx_destroy(context);
        printf("Error:%d: Node is unavailable\n", id);
        return -1;
    }
    
    add_pending_operation(id, socket, context, CMD_EXEC);
    return 0;
} 