//
// Created by IvanBrekman on 09.11.2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "config.h"
#include "libs/baselib.h"
#include "libs/file_funcs.h"

#include "tree.h"

int main(int argc, char** argv) {
    char* source_file = (char*) calloc(MAX_FILEPATH_SIZE, sizeof(char));
    int game_mode     = poisons::UNINITIALIZED_INT;

    if (argc >= 3) {
        source_file = argv[1];
        game_mode   = atoi(argv[2]);
    } else {
        printf("Input path to souce file\n");
        scanf("%s", source_file);

        printf("Input game mode (1-4)\n");
        while (scanf("%d", &game_mode) != 1 || game_mode < 1 || game_mode > 4) {
            printf("Inpit correct game mode (1-4)\n");
            while(getchar() != '\n') continue;
        }
    }
#if 1
    Node node = { };
    node_ctor(&node, NULL, (node_t)"Это животное?");
    print_node(&node);
    Node_dump(&node, "Check dump");

    Node node1 = { };
    node_ctor(&node1, NULL, (node_t)"Оно красивое?");

    Node node2 = { };
    node_ctor(&node2, NULL, (node_t)"Ведет матан?");

    Node node3 = { };
    node_ctor(&node3, NULL, (node_t)"Полторашка");

    Node node4 = { };
    node_ctor(&node4, NULL, (node_t)"Таракан");

    Node node5 = { };
    node_ctor(&node5, NULL, (node_t)"Лукашов");

    Node node6 = { };
    node_ctor(&node6, NULL, (node_t)"Богданов");

    Tree tree = { };
    tree_ctor(&tree);

    ADD_CHILD(tree, node, node1, -1);
    ADD_CHILD(tree, node, node2,  1);
    ADD_CHILD(tree, *node.left,  node3, -1);
    ADD_CHILD(tree, *node.left,  node4,  1);
    ADD_CHILD(tree, *node.right, node5, -1);
    ADD_CHILD(tree, *node.right, node6,  1);
#endif
#if 0
    Node node = { };
    node_ctor(&node, NULL, (node_t)"1");
    print_node(&node);
    Node_dump(&node, "Check dump");

    Node node1 = { };
    node_ctor(&node1, NULL, (node_t)"2");

    Node node2 = { };
    node_ctor(&node2, NULL, (node_t)"3");

    Node node3 = { };
    node_ctor(&node3, NULL, (node_t)"4");

    Node node4 = { };
    node_ctor(&node4, NULL, (node_t)"5");

    Tree tree = { };
    tree_ctor(&tree);

    ADD_CHILD(tree, node, node1, -1);
    ADD_CHILD(tree, node, node2,  1);
    ADD_CHILD(tree, *node.left,  node3,  1);
    ADD_CHILD(tree, *node.left->right,  node4, -1);
#endif

    //node1.parent = (Node*)0x7ffedf05d370;
    // *node1.parent;

    tree.root = &node;
    Tree_dump(&tree, "Check dump");

    // FILE* graph_log = open_file("log.html", "w");
    // Tree_dump_graph(&tree, "Check graph dump", graph_log);
    // fclose(graph_log);

    write_tree_to_file(&tree, "db/test.txt");

    printf("-------------------------------------------------------------------\n");
    getchar();
    int result = read_tree_from_file(&tree, "db/test.txt");
    if (result != -1) {
        printf("Error in reading tree data. Possible position of error: %d\n", result);
    }

    FILE* graph_log = open_file("log.html", "w");
    Tree_dump_graph(&tree, "Check graph dump", graph_log);
    fclose(graph_log);

    if (argc < 3) {
        FREE_PTR(source_file, char);
    }

    return 0;
}
