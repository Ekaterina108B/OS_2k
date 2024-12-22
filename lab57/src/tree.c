#include "tree.h"

TreeNode* create_node(int id, pid_t pid, const char* endpoint, TreeNode* parent) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if(node == NULL){ return NULL; }
    node->id = id;
    node->pid = pid;
    strncpy(node->endpoint, endpoint, sizeof(node->endpoint) - 1);
    node->endpoint[sizeof(node->endpoint)-1] = '\0';
    node->left_socket = NULL;
    node->right_socket = NULL;
    node->parent_socket = NULL;
    node->left = NULL;
    node->right = NULL;
    node->parent = parent;
    return node;
}

TreeNode* insert_node(TreeNode* root, int id, pid_t pid, const char* endpoint, int parent_id) {
    if(root == NULL){
        return create_node(id, pid, endpoint, NULL);
    }

    TreeNode* parent_node = NULL;
    TreeNode* current = root;
    while(current != NULL){
        parent_node = current;

        if(current->id == parent_id || parent_id == -1){
            if(id < current->id){
                current = current->left;
            } else {
                current = current->right;
            }
        } else {
            if(id < current->id){ current = current->left; }
            else { current = current->right; }
        }
    }

    TreeNode* new_node = create_node(id, pid, endpoint, parent_node);
    if(new_node == NULL){ return NULL; }
    if(id < parent_node->id){ parent_node->left = new_node; }
    else { parent_node->right = new_node; }

    return root;
}

TreeNode* find_node(TreeNode* root, int id) {
    if(root == NULL || root->id == id){ return root; }

    TreeNode* found = find_node(root->left, id);
    if(found != NULL) { return found; }
    return find_node(root->right, id);
}

TreeNode* find_min_node(TreeNode* node) {
    TreeNode* current = node;
    while(current && current->left != NULL){ current = current->left; }
    return current;
}

TreeNode* delete_node_recursive(TreeNode* root, int id) {
    if (root == NULL) { return root; }

    if(id < root->id){
        root->left = delete_node_recursive(root->left, id);
        if(root->left != NULL){root->left->parent = root; }
    } else if(id > root->id) {
        root->right = delete_node_recursive(root->right, id);
        if(root->right != NULL){ root->right->parent = root; }
    } else {
        if(root->left == NULL){
            TreeNode* temp = root->right;
            if(temp != NULL){ temp->parent = root->parent; }
            free(root);
            return temp;
        } else if(root->right == NULL){
            TreeNode* temp = root->left;
            if(temp != NULL){ temp->parent = root->parent; }
            free(root);
            return temp;
        }

        TreeNode* temp = find_min_node(root->right);
        root->id = temp->id;
        root->pid = temp->pid;
        strncpy(root->endpoint, temp->endpoint, sizeof(root->endpoint) - 1);
        root->endpoint[sizeof(root->endpoint)-1] = '\0';
        root->right = delete_node_recursive(root->right, temp->id);
        if(root->right != NULL){ root->right->parent = root; }
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
    if(root != NULL){
        kill_tree(root->left);
        kill_tree(root->right);
        kill(root->pid, SIGTERM);
    }
}

void free_tree(TreeNode* root) {
    if(root != NULL){
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}