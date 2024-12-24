#include <gtest/gtest.h>

extern "C" {
    #include "tree.h"
}

class TreeTest : public ::testing::Test {
protected:
    TreeNode* root;

    void SetUp() override {
        root = nullptr;
    }

    void TearDown() override {
        free_tree(root);
    }
};

TEST_F(TreeTest, InsertNode) {
    root = insert_node(root, 10, 1234, "endpoint1", -1);
    root = insert_node(root, 5, 1235, "endpoint2", 10);
    root = insert_node(root, 15, 1236, "endpoint3", 10);

    EXPECT_EQ(root->id, 10);
    EXPECT_EQ(root->left->id, 5);
    EXPECT_EQ(root->right->id, 15);
}

TEST_F(TreeTest, FindNode) {
    root = insert_node(root, 10, 1234, "endpoint1", -1);
    root = insert_node(root, 5, 1235, "endpoint2", 10);
    root = insert_node(root, 15, 1236, "endpoint3", 10);

    TreeNode* node = find_node(root, 5);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->id, 5);

    node = find_node(root, 20);
    EXPECT_EQ(node, nullptr);
}