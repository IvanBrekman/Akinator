//
// Created by IvanBrekman on 09.11.2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "config.h"
#include "libs/baselib.h"

#include "akinator.h"

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

    if (argc < 3) {
        FREE_PTR(source_file, char);
    }

    return 1;
}
