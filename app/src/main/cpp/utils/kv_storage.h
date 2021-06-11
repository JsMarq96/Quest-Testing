//
// Created by jsmar on 12/06/2021.
//

#ifndef QUEST_DEMO_KV_STORAGE_H
#define QUEST_DEMO_KV_STORAGE_H

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

//**
// A simple key value storage utility using a Radix tree
// by Juan S. Marquerie
// */

inline int string_similarity(char* str1,
                             char* str2) {
    int count = 0;
    for(; *str1 == *str2 && *str1 != '\0'; str1++, str2++) {
        count++;
    }
    return count;
}


union uKVStorage {
    char str[30];
    int integer;
    float floating_point;
};

struct sRadNode {
    char        *key          = NULL;
    int        key_len        = 0;
    uKVStorage *result        = NULL;

    sRadNode *children[256];
    bool is_full[256]         = { false };
};

inline void RN_init(sRadNode *node) {
    memset(node->is_full, false, 256);
    node->key          = NULL;
    node->key_len        = 0;
    node->result        = NULL;
}

inline bool RadNode_is_leaf(sRadNode *node) {
    return node->result != NULL;
}

inline void Rad_Node_add(sRadNode *node,
                         const char *key,
                         const int key_len,
                         uKVStorage *to_store) {
    int index = (int) *key;

    if (!node->is_full[index]) {
        sRadNode *new_child = (sRadNode*) malloc(sizeof(sRadNode));
        RN_init(new_child);
        new_child->key_len = key_len;
        new_child->key = (char*) malloc(key_len + 1);
        strcpy(new_child->key, key);

        node->is_full[index] = true;
        node->children[index] = new_child;

        return;
    }

    sRadNode *prev_node = NULL;
    sRadNode *it_node = node->children[index];
    char *res_key = (char*) key;
    int res_key_len = key_len;
    bool t;
    while(t) {
        int old_index = (int) *it_node->key;
        int similarity = string_similarity(res_key, it_node->key);

        // NOTE: needs a more case, when the key is bigger than the it_node->key
        if (similarity == it_node->key_len) {
            // Reduce the key and teh key len, and iterate to the next node
            prev_node = it_node;
            res_key += similarity;
            res_key_len -= similarity;
            it_node = it_node->children[(int) *res_key];
        } else {
            // Since the similarity is not equal, it creates a bifurcation on the tree,
            // storing the result

            res_key += similarity;
            res_key_len -= similarity;

            // Create the new "root" node
            sRadNode *new_root = (sRadNode*) malloc(sizeof(sRadNode));
            memcpy(new_root, it_node, sizeof(sRadNode));
            new_root->key_len = similarity;
            new_root->key = (char*) malloc(new_root->key_len + 1);
            memcpy(new_root->key, it_node->key, similarity);

            // Swap the old key with the new, sliced key
            it_node->key_len -= similarity;
            char *it_node_new_key = (char*) malloc(it_node->key_len + 1);
            strcpy(it_node_new_key, it_node->key + similarity);
            free(it_node->key);
            it_node->key = it_node_new_key;

            // Create the new leaf node
            sRadNode *new_leaf = (sRadNode*) malloc(sizeof(sRadNode));
            RN_init(new_leaf);
            new_leaf->key_len = res_key_len;
            new_leaf->key = (char*) malloc(res_key_len + 1);
            strcpy(new_leaf->key, res_key);
            new_leaf->result = to_store;

            // Add the new leafs to the new root
            new_root->children[(int) *new_leaf->key] = new_leaf;
            new_root->children[(int) *it_node->key] = it_node;
            prev_node->children[old_index] = new_root;
        }

    }
}

struct sKVStorage {
    sRadNode  *root_node;
};

void KVS_init(sKVStorage *storage) {
    storage->root_node = (sRadNode*) malloc(sizeof(sRadNode));
    RN_init(storage->root_node);
}

inline void KVS_add(sKVStorage *kv_storage, const char *key,
                    const int key_len,
                    const int to_store) {

    uKVStorage result{};
    result.integer = to_store;
    Rad_Node_add(kv_storage->root_node,
                 key,
                 key_len,
                 &result);
}

inline void KVS_add(sKVStorage *kv_storage, const char *key,
                    const int key_len,
                    const float to_store) {
    uKVStorage result{};
    result.floating_point = to_store;
    Rad_Node_add(kv_storage->root_node,
                 key,
                 key_len,
                 &result);
}

inline void KVS_add(sKVStorage *kv_storage, const char *key,
                    const int key_len,
                    const char to_store[30]) {
    uKVStorage result{};
    // This is kinda yuck yuck, but since both of them are only allowed to go out of 30 chars...
    // should be fine right..? (no, but will fix it later)
    strcpy(&result.str[0], &to_store[0]);
    Rad_Node_add(kv_storage->root_node,
                 key,
                 key_len,
                 &result);
}

#endif //QUEST_DEMO_KV_STORAGE_H
