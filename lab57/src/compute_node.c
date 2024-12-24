#include "compute_node.h"

static int node_id;
static void* left_socket = NULL;
static void* right_socket = NULL;
static bool left_child_alive = false;
static bool right_child_alive = false;
static int left_child_id = -1;
static int right_child_id = -1;


void* create_socket_with_timeouts(void) {
    void* socket = zmq_socket(zmq_ctx_new(), ZMQ_REQ);
    int timeout = 250;
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
    void* socket = create_socket_with_timeouts();
    connect_to_child(socket, child_id);
    return socket;
}

bool try_child_socket(void** socket, Message* msg, const char* side) {
    monitor_children();
    
    if(*socket){
        if((strcmp(side, "left") == 0 && !left_child_alive) ||
            (strcmp(side, "right") == 0 && !right_child_alive)){
            return false;
        }
        
        int child_id;
        if(strcmp(side, "left") == 0){
            child_id = left_child_id;
        } else {
            child_id = right_child_id;
        }
        if(child_id == -1){ return false; }

        send_message(*socket, msg);
        
        Message response = {0};
        int recv_result = receive_message(*socket, &response);
        if(recv_result <= 0){
            zmq_close(*socket);
            *socket = create_socket_with_timeouts();
            connect_to_child(*socket, child_id);
            return false;
        } else if(response.is_response == 1){
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
    if(msg->target_id == node_id){
        if(strcmp(msg->command, CMD_CREATE) == 0){
            if(left_socket != NULL && right_socket != NULL){
                strcpy(msg->data, "Node is full");
                msg->is_response = -1;
                return;
            }
            
            if(left_socket != NULL){
                right_socket = setup_child_connection(msg->create_child_id);
                right_child_id = msg->create_child_id;
                right_child_alive = true;
                strcpy(msg->data, "OK");
                msg->is_response = 1;
                return;
            }
            
            if(right_socket != NULL){
                left_socket = setup_child_connection(msg->create_child_id);
                left_child_id = msg->create_child_id;
                left_child_alive = true;
                strcpy(msg->data, "OK");
                msg->is_response = 1;
                return;
            }
            
            if(msg->create_child_id < node_id){
                left_socket = setup_child_connection(msg->create_child_id);
                left_child_id = msg->create_child_id;
                left_child_alive = true;
            } else {
                right_socket = setup_child_connection(msg->create_child_id);
                right_child_id = msg->create_child_id;
                right_child_alive = true;
            }
            strcpy(msg->data, "OK");
            msg->is_response = 1;
        } else if(strcmp(msg->command, CMD_PING) == 0){
            strcpy(msg->data, "1");
            msg->is_response = 1;
        } else if (strcmp(msg->command, CMD_EXEC) == 0) {
            char* result = find_substring(msg->data, msg->data + strlen(msg->data) + 1);
            strcpy(msg->data, result);
            msg->is_response = 1;
        }
    } else {
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
        monitor_children();
        Message msg;
        int recv_result = receive_message(socket, &msg);
        if(recv_result > 0){
            handle_message(&msg);
            send_message(socket, &msg);
        }
    }
    zmq_close(socket);
    zmq_ctx_destroy(context);
}

bool ping_child(void** socket, int child_id) {
    Message msg = {0};
    strcpy(msg.command, CMD_PING);
    msg.source_id = node_id;
    msg.target_id = child_id;
    
    if (send_message(*socket, &msg) <= 0) {
        return false;
    }
    Message response = {0};
    if (receive_message(*socket, &response) <= 0) {
        return false;
    }
    return response.is_response == 1;
}

void monitor_children(void){
    if(left_socket != NULL && left_child_id != -1) {
        left_child_alive = ping_child(&left_socket, left_child_id);
    }
    if (right_socket != NULL && right_child_id != -1) {
        right_child_alive = ping_child(&right_socket, right_child_id);
    }
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