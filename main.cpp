//
// Created by IvanBrekman on 09.11.2021.
//

#include <cstdlib>

int main(void) {
    system("gcc game/akinator.cpp tree.cpp libs/baselib.cpp libs/file_funcs.cpp -lstdc++ -o game/game.out");
    system("./game/game.out");

    return 0;
}
