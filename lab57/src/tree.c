#include "tree.h"

TreeNode* create_node(int id, pid_t pid, const char* endpoint, TreeNode* parent) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if(node == NULL){ return NULL; }
    node->id = id;
    node->pid = pid;
    strncpy(node->endpoint, endpoint, sizeof(node->endpoint) - 1);
    node->endpoint[sizeof(node->endpoint)-1] = '\0';
    node->left = NULL;
    node->right = NULL;
    node->parent = parent;
    return node;
}

TreeNode* insert_node(TreeNode* root, int id, pid_t pid, const char* endpoint, int parent_id) {
    if(root == NULL){
        return create_node(id, pid, endpoint, NULL);
    }

    if(parent_id == -1){
        if(id < root->id){
            root->left = insert_node(root->left, id, pid, endpoint, parent_id);
        } else if(id>root->id){
            root->right = insert_node(root->right, id, pid, endpoint, parent_id);
        }

    } else if(root->id == parent_id){
        if(root->left != NULL && root->right != NULL){ return root; }
        else if(root->left == NULL && root->right != NULL){
            root->left = create_node(id, pid, endpoint, root);
        } else if(root->left != NULL && root->right == NULL){
            root->right = create_node(id, pid, endpoint, root);
        } else {
            if(id < parent_id){
                root->left = create_node(id, pid, endpoint, root);
            } else {
                root->right = create_node(id, pid, endpoint, root);                }
        }
    } else {
        TreeNode* left_result = NULL;
        TreeNode* right_result = NULL;

        if(root->left){
            left_result = insert_node(root->left, id, pid, endpoint, parent_id);
            if(left_result != root->left){ root->left = left_result; }
        }
        if(root->right){
            right_result = insert_node(root->right, id, pid, endpoint, parent_id);
            if(right_result != root->right){ root->right = left_result; }
        }
    }
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
        if(root->left != NULL){ root->left->parent = root; }
    } else if(id > root->id) {
        root->right = delete_node_recursive(root->right, id);
        if(root->right != NULL){ root->right->parent = root; }
    } else {
        if(root->left == NULL && root->right == NULL){
            free(root);
            return NULL;
        } else if(root->left == NULL){
            TreeNode* temp = root->right;
            free(root);
            return temp;
        } else if(root->right == NULL){
            TreeNode* temp = root->left;
            free(root);
            return temp;
        }

        TreeNode* temp = find_min_node(root->right);
        root->id = temp->id;
        root->pid = temp->pid;
        strncpy(root->endpoint, temp->endpoint, sizeof(root->endpoint) - 1);
        root->endpoint[sizeof(root->endpoint)-1] = '\0';
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

void print_tree_node(TreeNode* node, int level, char* prefix, int is_left) {
    if(node == NULL) { return; }

    printf("%s", prefix);
    printf("%c", is_left ? '\\' : '/');
    printf("--%d\n", node->id);

    char new_prefix[256];
    strcpy(new_prefix, prefix);
    
    if(is_left){ strcat(new_prefix, "  "); }
    else { strcat(new_prefix, "| "); }

    if(node->left){
        print_tree_node(node->left, level + 1, new_prefix, 0);
    }
    if(node->right){
        print_tree_node(node->right, level + 1, new_prefix, 1);
    }
}

void print_tree(TreeNode* root) {
    if(root == NULL){
        printf("Empty tree\n");
        return;
    }

    printf("%d\n", root->id);
    char prefix[256] = "";
    if(root->left){
        print_tree_node(root->left, 1, prefix, 0);
    }
    if(root->right){
        print_tree_node(root->right, 1, prefix, 1);
    }
}