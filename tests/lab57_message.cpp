#include <gtest/gtest.h>
#include <zmq.h>

// Use extern "C" to properly link C functions
extern "C" {
    #include "../lab57/include/message.h"
}

class MessageTest : public ::testing::Test {
protected:
    void* context;
    void* sender;
    void* receiver;

    void SetUp() override {
        context = zmq_ctx_new();
        sender = zmq_socket(context, ZMQ_PAIR);
        receiver = zmq_socket(context, ZMQ_PAIR);

        zmq_bind(receiver, "inproc://test");
        zmq_connect(sender, "inproc://test");
    }

    void TearDown() override {
        zmq_close(sender);
        zmq_close(receiver);
        zmq_ctx_destroy(context);
    }
};

TEST_F(MessageTest, SendMessage) {
    Message msg;
    strcpy(msg.command, CMD_PING);
    msg.source_id = 1;
    msg.target_id = 2;
    strcpy(msg.data, "Test data");

    send_message(sender, &msg);

    Message received_msg;
    receive_message(receiver, &received_msg);

    EXPECT_STREQ(received_msg.command, CMD_PING);
    EXPECT_EQ(received_msg.source_id, 1);
    EXPECT_EQ(received_msg.target_id, 2);
    EXPECT_STREQ(received_msg.data, "Test data");
}