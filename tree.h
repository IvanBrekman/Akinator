//
// Created by IvanBrekman on 09.11.2021.
//

#ifndef TREE_TREEH
#define TREE_TREEH

#include <cstdio>
#include <cstdlib>
#include <list>

#define ASSERT_OK(obj, type, reason, ret) {                                         \
    if (VALIDATE_LEVEL >= WEAK_VALIDATE && type ## _error(obj)) {                   \
        type ## _dump(obj, reason);                                                 \
        if (VALIDATE_LEVEL >= HIGHEST_VALIDATE) {                                   \
            LOG_DUMP(obj, reason, type ## _dump)                                    \
        }                                                                           \
        ASSERT_IF(0, "verify failed", ret);                                         \
    } else if (type ## _error(obj)) {                                               \
        errno = type ## _error(obj);                                                \
        return ret;                                                                 \
    }                                                                               \
}

#define ADD_CHILD(tree, parent, child, type_child) {                                \
    int result = add_child(&parent, &child, type_child);                            \
    if (result >= 1) tree.size++;                                                   \
    tree.depth = tree.depth >= result ? tree.depth : result;                        \
}

#define IND_STRCHR(ptr, index, sym) (int)(strchr(&(ptr)[index], sym) - (ptr))
#define VALID_NODE(ptr) (VALID_PTR(ptr) || (ptr) == NULL)

const int MAX_FILEPATH_SIZE = 50;
const int MAX_NODE_STR_SIZE = 100;
const long CANARY           = 0x5AFEA2EA; // SAFE AREA
const long POISON_CANARY    = 0xDEADA2EA; // DEAD AREA

typedef char* node_t;
const node_t INIT_VALUE   = (node_t)"";
const node_t DEINIT_VALUE = (node_t)poisons::FREED_ELEMENT;

struct Node {
    long left_canary  = POISON_CANARY;

    node_t data  = (node_t)poisons::UNINITIALIZED_INT;

    Node* parent = (Node*)poisons::UNINITIALIZED_PTR;
    Node* left   = (Node*)poisons::UNINITIALIZED_PTR;
    Node* right  = (Node*)poisons::UNINITIALIZED_PTR;

    int depth    = poisons::UNINITIALIZED_INT;

    long right_canary = POISON_CANARY;
};

struct Tree {
    Node* root = (Node*)poisons::UNINITIALIZED_PTR;

    int size  = -1;
    int depth = -1;
};

struct Node_Child {
    Node* node = { };
    int child_amount = 0;
};

struct NodeDesc {
    Node* node   = { };
    int is_left_child = -1;
};

enum errors {
    OK_               =  0,

    // Node errors---------
    INVALID_NODE_PTR =  -1,

    DAMAGED_LCANARY  =  -2,
    DAMAGED_RCANARY  =  -3,
    POISONED_LCANARY =  -4,
    POISONED_RCANARY =  -5,

    INVALID_PARENT   =  -6,
    INVALID_LEFT     =  -7,
    INVALID_RIGHT    =  -8,
    // --------------------

    // Tree errors---------
    INVALID_TREE_PTR = -10,

    INCORRECT_SIZE   = -11,
    INCORRECT_DEPTH  = -12
    // --------------------
};

int tree_ctor(Tree* tree);
int tree_dtor(Tree* tree);

int node_ctor(Node* node, Node* parent=NULL, node_t value=INIT_VALUE);
int node_dtor(Node* node);

const char* error_desc(int error_code);
int Tree_error(Tree* tree);
int Node_error(Node* node, int recursive_check=0);

int      add_child(Node* parent, Node* child, int is_left_child);
int        is_leaf(Node* node);
int   is_full_node(Node* node);
int  is_left_child(Node* node);
int is_right_child(Node* node);

Node* find_node_by_value(Tree* tree, node_t value, std::list<NodeDesc>* path, int (*value_comparator) (node_t val1, node_t val2));

int get_inorder_nodes(Node* node, std::list<Node*>* nodes);

int print_node(Node* node);
int  Node_dump(Node* node, const char* reason, FILE* log=stdout);
int  Tree_dump(Tree* tree, const char* reason, FILE* log=stdout);
int  Tree_dump_graph(Tree* tree, const char* reason, FILE* log, int show_parent_edge=0);

int  write_tree_to_file(Tree* tree, const char* filename);
int write_nodes_to_file(Node* node, FILE* file);
int read_tree_from_file(Tree* tree, const char* source_file);

int update_tree_depth_size(Tree* tree);
int analyze_tree_data(char* data, Tree* tree, std::list<Node_Child>* added_nodes, int* shift);
int analyze_func_debug(const char* data, int index, std::list<Node_Child>* added_nodes, const char* reason);

#endif // TREE_TREEH
