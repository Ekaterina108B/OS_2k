#include "tree.h"

TreeNode* create_node(int id, pid_t pid, const char* endpoint) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    node->id = id;
    node->pid = pid;
    strcpy(node->endpoint, endpoint);
    node->left = NULL;
    node->right = NULL;
    return node;
}

TreeNode* insert_node(TreeNode* root, int id, pid_t pid, const char* endpoint) {
    if(root == NULL){
        return create_node(id, pid, endpoint);
    }

    if(id < root->id){
        root->left = insert_node(root->left, id, pid, endpoint);
    } else if(id > root->id){
        root->right = insert_node(root->right, id, pid, endpoint);
    }

    return root;
}

TreeNode* find_node(TreeNode* root, int id) {
    if(root == NULL || root->id == id){ return root; }

    if(id < root->id){
        return find_node(root->left, id);
    }
    return find_node(root->right, id);
}

TreeNode* find_min_node(TreeNode* node) {
    TreeNode* current = node;
    while(current && current->left != NULL) {
        current = current->left;
    }
    return current;
}

TreeNode* delete_node_recursive(TreeNode* root, int id) {
    if(root == NULL){ return root; }
    
    if(id < root->id) {
        root->left = delete_node_recursive(root->left, id);
    } else if(id > root->id) {
        root->right = delete_node_recursive(root->right, id);
    } else {
        if(root->left == NULL) {
            TreeNode* temp = root->right;
            free(root);
            return temp;
        } else if(root->right == NULL) {
            TreeNode* temp = root->left;
            free(root);
            return temp;
        }

        TreeNode* temp = find_min_node(root->right);
        root->id = temp->id;
        root->pid = temp->pid;
        strcpy(root->endpoint, temp->endpoint);
        root->right = delete_node_recursive(root->right, temp->id);
    }
    return root;
}

void delete_node(TreeNode* root, int id) {
    root = delete_node_recursive(root, id);
}

bool is_node_exists(TreeNode* root, int id) {
    return find_node(root, id) != NULL;
}

void kill_tree(TreeNode* root) {
    if(root != NULL) {
        kill_tree(root->left);
        kill_tree(root->right);
        kill(root->pid, SIGTERM);
    }
}

void free_tree(TreeNode* root) {
    if(root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}