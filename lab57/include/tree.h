#ifndef TREE_H
#define TREE_H

#include <stdbool.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

typedef struct TreeNode {
    int id;
    pid_t pid;
    char endpoint[64];
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

extern TreeNode* root;

TreeNode* create_node(int id, pid_t pid, const char* endpoint);
TreeNode* insert_node(TreeNode* root, int id, pid_t pid, const char* endpoint);
TreeNode* find_node(TreeNode* root, int id);
TreeNode* find_min_node(TreeNode* node);
TreeNode* delete_node_recursive(TreeNode* root, int id);
void delete_node(TreeNode* root, int id);
bool is_node_exists(TreeNode* root, int id);
void kill_tree(TreeNode* root);
void free_tree(TreeNode* root);

#endif