#include "compute_node.h"

static int node_id;
static void* left_socket;
static void* right_socket;

void* create_socket(int timeout) {
    void* socket = zmq_socket(zmq_ctx_new(), ZMQ_REQ);
    int linger = 0;
    zmq_setsockopt(socket, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    zmq_setsockopt(socket, ZMQ_SNDTIMEO, &timeout, sizeof(timeout));
    zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(linger));
    return socket;
}

void connect_to_child(void* socket, int child_id) {
    char child_endpoint[64];
    sprintf(child_endpoint, "tcp://localhost:%d", 5555 + child_id);
    zmq_connect(socket, child_endpoint);
}

void* setup_child_connection(int child_id) {
    void* socket = create_socket(250);
    connect_to_child(socket, child_id);
    return socket;
}

bool try_child_socket(void** socket, Message* msg, const char* side) {
    if(*socket){
        int child_id;
        if(strcmp(side, "left") == 0){
            child_id = node_id - 1;
        } else {
            child_id = node_id + 1;
        }

        if(send_message(*socket, msg) <= 0){
            zmq_close(*socket);
            *socket = create_socket(250);
            connect_to_child(*socket, child_id);
            return false;
        }
        
        Message response = {0};
        int recv_result = receive_message(*socket, &response);
        if(recv_result <= 0){
            zmq_close(*socket);
            *socket = create_socket(250);
            connect_to_child(*socket, child_id);
            return false;
        }
        
        if(response.is_response == 1){
            *msg = response;
            return true;
        }
    }
    return false;
}

char* find_substring(const char* text, const char* pattern) {
    static char result[MAX_BUFFER_SIZE];
    int text_len = strlen(text);
    int pattern_len = strlen(pattern);
    int found = 0;
    result[0] = '\0';

    for(int i=0; i<=text_len-pattern_len; ++i){
        if(strncmp(&text[i], pattern, pattern_len) == 0){
            char pos[16];
            if(found > 0){ strcat(result, ";"); }
            sprintf(pos, "%d", i);
            strcat(result, pos);
            ++found;
        }
    }
    if(!found){ strcpy(result, "-1"); }
    return result;
}

void handle_message(Message* msg) {
    // Системная проверка
    // printf("Node %d received message: cmd=%s, target=%d, create_child=%d\n", 
    //        node_id, msg->command, msg->target_id, msg->create_child_id);

    if(msg->target_id == node_id){
        if(strcmp(msg->command, CMD_CREATE) == 0){
            // printf("Node %d handling create for child %d\n", node_id, msg->create_child_id);
            
            if((left_socket && msg->create_child_id == node_id - 1) || (right_socket && msg->create_child_id == node_id + 1)){
                strcpy(msg->data, "Error: Child already connected");
                msg->is_response = -1;
                return;
            }
            
            if(left_socket != NULL && right_socket != NULL){
                strcpy(msg->data, "Error: Node is full");
                msg->is_response = -1;
                return;
            }
            
            if(left_socket != NULL){
                right_socket = setup_child_connection(msg->create_child_id);
                strcpy(msg->data, "OK");
                msg->is_response = 1;
                return;
            }
            
            if(right_socket != NULL){
                left_socket = setup_child_connection(msg->create_child_id);
                strcpy(msg->data, "OK");
                msg->is_response = 1;
                return;
            }
        
            if(msg->create_child_id < node_id){
                left_socket = setup_child_connection(msg->create_child_id);
            } else {
                right_socket = setup_child_connection(msg->create_child_id);
            }
            strcpy(msg->data, "OK");
            msg->is_response = 1;

        } else if(strcmp(msg->command, CMD_PING) == 0){
            strcpy(msg->data, "1");
            msg->is_response = 1;

        } else if(strcmp(msg->command, CMD_EXEC) == 0){
            char* result = find_substring(msg->data, msg->data + strlen(msg->data) + 1);
            strcpy(msg->data, result);
            msg->is_response = 1;
        }

    } else {
        // Тут можно посмотреть, что сообщения действительно переотправляются только детям
        // printf("Node %d trying both children\n", node_id);
        // printf("left_socket: %p\n", left_socket);
        // printf("right_socket: %p\n", right_socket);
        
        bool found = false;
        found = try_child_socket(&left_socket, msg, "left");
        if(!found){
            found = try_child_socket(&right_socket, msg, "right");
        }

        if(!found){
            strcpy(msg->data, "Node is unavailable");
            msg->is_response = -1;
        }
    }
}

void start_compute_node(int id) {
    node_id = id;
    void* context = zmq_ctx_new();
    void* socket = zmq_socket(context, ZMQ_REP);
    
    char endpoint[64];
    sprintf(endpoint, "tcp://*:%d", 5555 + id);
    zmq_bind(socket, endpoint);

    while(1){
        Message msg;
        int recv_result = receive_message(socket, &msg);
        if(recv_result > 0){
            handle_message(&msg);
            send_message(socket, &msg);
        }
    }
}

int check_parent(void* context, int parent_id, int id) {
    if(parent_id < 0){ return 1; }
    
    void* parent_socket = zmq_socket(context, ZMQ_REQ);
    char parent_endpoint[64];
    sprintf(parent_endpoint, "tcp://localhost:%d", 5555 + parent_id);
    int timeout = 1000;
    int linger = 0;
    zmq_setsockopt(parent_socket, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    zmq_setsockopt(parent_socket, ZMQ_SNDTIMEO, &timeout, sizeof(timeout));
    zmq_setsockopt(parent_socket, ZMQ_LINGER, &linger, sizeof(linger));
    if(zmq_connect(parent_socket, parent_endpoint) != 0){
        zmq_close(parent_socket);
        return 0;
    }

    Message msg = {0};
    strcpy(msg.command, CMD_PING);
    msg.source_id = id;
    msg.target_id = parent_id;
    if(send_message(parent_socket, &msg) <= 0){
        zmq_close(parent_socket);
        return 0;
    }

    if(receive_message(parent_socket, &msg) <= 0){
        zmq_close(parent_socket);
        return 0;
    }

    zmq_close(parent_socket);
    return 1;
}

int main(int argc, char *argv[]) {
    if(argc != 3){
        fprintf(stderr, "Usage: %s <id> <parent_id>\n", argv[0]);
        return 1;
    }

    int id = atoi(argv[1]);
    start_compute_node(id);
    return 0;
} 