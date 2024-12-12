#include "compute_node.h"

int check_parent(void* context, int parent_id, int id);

char* find_substring(const char* text, const char* pattern) {
    static char result[MAX_BUFFER_SIZE];
    int text_len = strlen(text);
    int pattern_len = strlen(pattern);
    int found = 0;
    result[0] = '\0';
    for(int i=0; i<=text_len-pattern_len; ++i) {
        if(strncmp(&text[i], pattern, pattern_len) == 0) {
            char pos[16];
            if(found > 0) {
                strcat(result, ";");
            }
            sprintf(pos, "%d", i);
            strcat(result, pos);
            found++;
        }
    }
    if(!found) { strcpy(result, "-1"); }
    return result;
}

void start_compute_node(int id, int parent_id) {
    void* context = zmq_ctx_new();
    void* socket = zmq_socket(context, ZMQ_REP);
    char endpoint[64];
    sprintf(endpoint, "tcp://*:%d", 5555 + id);
    zmq_bind(socket, endpoint);
    time_t last_check = 0;
    const int CHECK_INTERVAL = 1;

    while(1) {
        time_t now = time(NULL);
        if(now - last_check >= CHECK_INTERVAL) {
            if(!check_parent(context, parent_id, id)) {
                zmq_close(socket);
                zmq_ctx_destroy(context);
                exit(1);
            }
            last_check = now;
        }

        int timeout = 100;
        zmq_setsockopt(socket, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
        Message msg;
        int recv_result = receive_message(socket, &msg);
        if(recv_result > 0) {
            if(!check_parent(context, parent_id, id)) {
                zmq_close(socket);
                zmq_ctx_destroy(context);
                exit(1);
            }
            if(strcmp(msg.command, CMD_EXEC) == 0) {
                char* result = find_substring(msg.data, msg.data + strlen(msg.data) + 1);
                strcpy(msg.data, result);
                send_message(socket, &msg);
            } else if(strcmp(msg.command, CMD_PING) == 0) {
                strcpy(msg.data, "1");
                send_message(socket, &msg);
            }
        }
    }
    zmq_close(socket);
    zmq_ctx_destroy(context);
}

int check_parent(void* context, int parent_id, int id) {
    if(parent_id < 0) { return 1; }
    
    void* parent_socket = zmq_socket(context, ZMQ_REQ);
    char parent_endpoint[64];
    sprintf(parent_endpoint, "tcp://localhost:%d", 5555 + parent_id);
    
    int timeout = 1000;
    int linger = 0;
    zmq_setsockopt(parent_socket, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    zmq_setsockopt(parent_socket, ZMQ_SNDTIMEO, &timeout, sizeof(timeout));
    zmq_setsockopt(parent_socket, ZMQ_LINGER, &linger, sizeof(linger));
    if(zmq_connect(parent_socket, parent_endpoint) != 0) {
        zmq_close(parent_socket);
        return 0;
    }

    Message msg = {0};
    strcpy(msg.command, CMD_PING);
    msg.source_id = id;
    msg.target_id = parent_id;
    if(send_message(parent_socket, &msg) <= 0) {
        zmq_close(parent_socket);
        return 0;
    }
    
    if(receive_message(parent_socket, &msg) <= 0) {
        zmq_close(parent_socket);
        return 0;
    }

    zmq_close(parent_socket);
    return 1;
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <id> <parent_id>\n", argv[0]);
        return 1;
    }
    int id = atoi(argv[1]);
    int parent_id = atoi(argv[2]);
    start_compute_node(id, parent_id);
    return 0;
} 