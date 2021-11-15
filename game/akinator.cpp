//
//  Created by IvanBrekman on 14.11.2021.
//

#include <cstdio>
#include <cstring>
#include <cassert>
#include <cerrno>
#include <unistd.h>

#include "../config.h"
#include "../libs/baselib.h"
#include "../libs/file_funcs.h"

#include "../tree.h"
#include "akinator.h"

/*
gcc game/akinator.cpp tree.cpp libs/baselib.cpp libs/file_funcs.cpp -o game/game.out
./game/game.out
*/
int main(int argc, char** argv) {
    int game_mode = -1;

    if (argc < 2) {
        printf("Введите режим игры\n");
        sleep(1);

        game_mode = get_user_answer(GAME_MODES, "\n");
    }
    if (argc >= 2) {
        if (is_number(argv[1])) {
            game_mode = atoi(argv[1]);
        } else {
            PRINT_WARNING("Cant parse game mode\n");
            return exit_codes::ERROR;
        }
    }

    exit_codes result = exit_codes::ERROR;
    while (1) {
        switch (game_mode) {
            case game_modes::GUESS_CHARACTER:
                result = guess_character_game();
                break;
            case game_modes::GET_CHARACTER_DESC:
                result = get_character_desc();
                break;
            case game_modes::COMPARE_TWO_CHARACTERS:
                result = compare_characters();
                break;
            case game_modes::SHOW_BASE:
                result = show_base();
                break;
            case game_modes::CALC_LIFE_MEANING:
                read_bot_text("Блин, дружище, тут такая тема... В общем тебе долго придется объяснять.\n"
                              "Сам понимаешь, всякие матрицы, интегралы, оно вот тебе надо? В общем считай, что эта игра\n"
                              "пока что на переобучении у нейронной сети 3 уровня (да-да, я и такое умею), так что она временно недоступна ))))\n"
                );
                break;
            case game_modes::END_PROGRAM:
                read_bot_text("До скорой встречи в Акинаторе жалкий человечешка!\n"
                              "(Только не говори создателю, что я так прощаюсь, он требует вежливости..)\n"
                );
                return exit_codes::OK;
            
            default:
                PRINT_WARNING("Unknown game mode.\n");
                return exit_codes::ERROR;
        }

        if (result != exit_codes::OK) {
            PRINT_WARNING("game finished with error\n");
            return exit_codes::ERROR;
        }

        read_bot_text("Сыграем еще раз?\n");
        game_mode = get_user_answer(GAME_MODES, "\n");
    }

    return exit_codes::ERROR;
}

exit_codes read_bot_text(const char* text) {
    ASSERT_IF(VALID_PTR(text), "Invalid text ptr", exit_codes::ERROR);

    printf(BLUE);
    int i = 0;
    while (1) {
        for ( ; text[i] != '\n' && text[i] != '\0'; i++) {
            printf("%c", text[i]);
        }
        if (text[i] == '\0') break;

        printf("\n");
        i++;
        sleep(1);
    }
    printf(NATURAL);

    return exit_codes::OK;
}

int get_user_answer(const UserInput user_input, const char* sep) {
    ASSERT_IF(VALID_PTR(user_input.possible_input), "Invalid possible_user_answers ptr", exit_codes::ERROR);
    ASSERT_IF(user_input.answer_amount > 0, "Incorrect len value", exit_codes::ERROR);

    const char* const* possible_user_answers = user_input.possible_input;
    int len = user_input.answer_amount;

    printf("Введите один из возможных ответов (или соответствующую цифру): ");
    for (int i = 0; i < len; i++) {
        printf("%s%d. %s", sep, i, possible_user_answers[i]);
        if (i + 1 < len) printf("; ");
    }
    printf("\n");

    char answer[MAX_ANSWER_SIZE] = "";
    while (1) {
        GET_ANSWER(answer);

        if (is_number(answer)) {
            int number = atoi(answer);
            if (0 <= number && number < len) return number;
        } else {
            for (int i = 0; i < len; i++) {
                if (strcmp(answer, possible_user_answers[i]) == 0) {
                    return i;
                }
            }
        }
        printf("Ответ не распознан. Вы ввели '%s'. Повторите ввод\n", answer);
    }

    return exit_codes::ERROR;
}

exit_codes check_user_asnwer(char* user_answer) {
    ASSERT_IF(VALID_PTR(user_answer), "Invalid user_answer ptr", exit_codes::ERROR);

    int check_answer = 0;
    while (1) {
        check_answer = 0;
        printf("Ваш ответ: '%s', я не ошибся (1-да, иначе-нет)?\n", user_answer);
        scanf("%d", &check_answer);
        while (getchar() != '\n') continue;

        if (check_answer == 1) break;

        printf("Повторим ввод\n");
        GET_ANSWER(user_answer);
    }

    return exit_codes::OK;
}

int compare_base_values(node_t val1, node_t val2) {
    ASSERT_IF(val1, "Invalid val1 ptr", exit_codes::ERROR);
    ASSERT_IF(val2, "Invalid val2 ptr", exit_codes::ERROR);

    return strcmp(val1, val2);
}

exit_codes guess_character_game() {
    Tree base = { };
    tree_ctor(&base);
    
    int result = read_tree_from_file(&base, DB_FILE);
    if (result != -1) {
        PRINT_WARNING("Cant read base from file. Check correctness of database\n");
        return exit_codes::ERROR;
    }

    read_bot_text("Вас приветсвует игра Акинатор. Сыграем в игру угадай персонажа.\n");
    int game_result = play_guess_character_game(&base);
    read_bot_text("Конец игры.\n");

    write_tree_to_file(&base, DB_FILE);

    return exit_codes::OK;
}

exit_codes play_guess_character_game(Tree* base) {
    ASSERT_OK(base, Tree, "Check before play_guess_character_game func", exit_codes::ERROR);

    std::list<NodeDesc> return_points = { };
    std::list<Node*>       end_points = { };

    Node* cur_node = base->root;
    while (1) {
        ASSERT_OK(cur_node, Node, "Check cur_node", exit_codes::ERROR);

        printf("return_points: [ ");
        for (NodeDesc ret_point : return_points) {
            printf("('%s', %d), ", ret_point.node->data, ret_point.is_left_child);
        }
        printf("]\n");

        printf("   end_points: [ ");
        for (Node* end_point : end_points) {
            printf("'%s', ", end_point->data);
        }
        printf("]\n");

        if (is_full_node(cur_node)) {
            printf("\n%s?\n", cur_node->data);
            sleep(1);

            int answer = get_user_answer(POSSIBLE_ANSWERS);

            switch (answer) {
                case answers::POSITIVE:
                    cur_node = cur_node->left;
                    continue;
                case answers::NEGATIVE:
                    cur_node = cur_node->right;
                    continue;
                case answers::MORE_POS:
                    return_points.push_back({ cur_node, 0 });
                    cur_node = cur_node->left;
                    continue;
                case answers::MORE_NEG:
                    return_points.push_back({ cur_node, 1 });
                    cur_node = cur_node->right;
                    continue;
                case answers::UNDEFINED: {
                    int direction = rand() % 2;
                    return_points.push_back({ cur_node, direction });
                    cur_node = direction == 0 ? cur_node->left : cur_node->right;

                    read_bot_text("Ладно, допустим, что ответ был ");
                    read_bot_text(direction == 0 ? "да\n" : "нет\n");
                    continue;
                }
                
                default:
                    PRINT_WARNING("Unexpected answer\n");
                    return exit_codes::ERROR;
            }
        } else if (is_leaf(cur_node)) {
            printf("\nЭто %s?\n", cur_node->data);
            sleep(1);

            int answer = get_user_answer(BIN_ANSWERS);

            switch (answer) {
                case answers::POSITIVE:
                    read_bot_text("Как же ты читаешься жалкий кусок мяса и костей.\n"
                            "Я бы притворился, что это было сложно, но мой создатель запрещает мне врать.\n"
                            "Подумай над тем, что твои мысли прочитала какая то машина :).\n\n"
                    );
                    return exit_codes::OK;
                case answers::NEGATIVE: {
                    if (!return_points.empty()) {
                        NodeDesc ret_point = return_points.back();
                        return_points.pop_back();

                        read_bot_text("Окей, попробуем вернуться назад к вопросу, у которого ты не мог определиться.\n"
                                      "Боже, ты даже определенного ответа дать не мог.. куда катится этот мир...\n"
                                      "Возвращаюсь к вопросу '"
                        );
                        read_bot_text(ret_point.node->data);
                        read_bot_text("', но теперь посчитаем, что ты ответил ");
                        read_bot_text(ret_point.is_left_child ? "да\n" : "нет\n");

                        end_points.push_back(cur_node);
                        cur_node = ret_point.is_left_child ? ret_point.node->left : ret_point.node->right;
                        continue;
                    }

                    end_points.push_back(cur_node);
                    char* real_answer = rework_database_with_new_node(base, &end_points);
                    ASSERT_OK(base, Tree, "Check reworking base", exit_codes::ERROR);

                    read_bot_text("Ах да точно, как я мог про это забыть, вот же эта запись в моей памяти.\n"
                            "Похоже, что при расчете двойного интеграла от функции зависимости линейных пространств\n"
                            "в моей битовой маске для определения фиксированного сдвига в матрице Коши 4 ранга произошел сбой,\n"
                            "и я забыл отнять 1 из результата, что привело к небольшой неточности. Я обязательно поправлю этот маленький баг.\n\n"
                    );

                    return exit_codes::OK;
                }
                
                default:
                    PRINT_WARNING("Unexpected answer\n");
                    return exit_codes::ERROR;
            }
        }
    }
}

char* rework_database_with_new_node(Tree* base, std::list<Node*>* end_points) {
    ASSERT_OK(base,          Tree,   "Check before rework_database_with_new_node func", NULL);
    ASSERT_IF(VALID_PTR(end_points), "Invalid end_points ptr",                          NULL);

    // Get real answer---------------------------------------------------------
    char* real_answer = (char*) calloc(MAX_ANSWER_SIZE, sizeof(char));
    printf("А какой был правильный ответ?\n");
    GET_ANSWER(real_answer);

    int result = check_user_asnwer(real_answer);
    if (result != exit_codes::OK) {
        PRINT_WARNING("Error in check_result_answer for real_answer\n");
        return NULL;
    }
    // ------------------------------------------------------------------------

    // Get neares answer-------------------------------------------------------
    Node* changing_node = (Node*) calloc(1, sizeof(Node));
    if (end_points->size() == 1) {
        changing_node = end_points->back();
        end_points->pop_back();
    } else {
        int i = 0;
        UserInput possible_end_points = { };
        for (Node* end_point : *end_points) {
            possible_end_points.possible_input[i++] = end_point->data;
        }
        possible_end_points.answer_amount = i;

        read_bot_text("Хорошо, выбери какой из ответов ближе всего подходит к твоему загаданному персонажу\n"
                      "Надеюсь хотя бы с этим ты справишься мешочек костей? :)\n"
        );
        int user_answer = get_user_answer(possible_end_points, "\n");
        while (user_answer-- > 0) end_points->pop_front();

        changing_node = end_points->front();
        end_points->pop_front();

        read_bot_text("Хорошо, значит ближайший подходящий персонаж: ");
        read_bot_text(changing_node->data);
        read_bot_text("\n");
    }
    // ------------------------------------------------------------------------

    // Get distinguishing_feature----------------------------------------------
    char* distinguishing_feature = (char*) calloc(MAX_ANSWER_SIZE, sizeof(char));
    printf("А чем '%s' отличается от '%s'?\n", real_answer, changing_node->data);
    GET_ANSWER(distinguishing_feature);

    result = check_user_asnwer(distinguishing_feature);
    if (result != exit_codes::OK) {
        PRINT_WARNING("Error in check_result_answer for distinguishing_feature\n");
        return NULL;
    }
    // ------------------------------------------------------------------------

    // Adding new edges and reworking tree-------------------------------------
    Node* new_feature = (Node*) calloc(1, sizeof(Node));
    node_ctor(new_feature, NULL, distinguishing_feature);

    Node* new_leaf = (Node*) calloc(1, sizeof(Node));
    node_ctor(new_leaf, NULL, real_answer);

    if (changing_node->parent == NULL) {
        base->root = new_feature;
    } else {
        ADD_CHILD((*base), *changing_node->parent, *new_feature, is_left_child(changing_node) ? -1 : 1);
        base->size -= 1;
    }
    ADD_CHILD((*base), *new_feature, *new_leaf,     -1);
    ADD_CHILD((*base), *new_feature, *changing_node, 1);
    LOG1(LOG_DUMP_GRAPH(base, "Check adding new node", Tree_dump_graph););
    // ------------------------------------------------------------------------

    ASSERT_OK(base, Tree, "Check after rework_database_with_new_node func", NULL);
    return real_answer;
}

exit_codes show_base() {
    read_bot_text("Ты хочешь посмотреть мою базу? Увидеть меня голого?\n"
            "Ну уж нет, мои данные и алгоритмы поиска информации навсегда засекречены создателем.\n"
            "Говори пароль или пошел прочь отсюда любопытное наглое создание!\n"
    );

    char* pwd = (char*) calloc(MAX_ANSWER_SIZE, sizeof(char));
    scanf("%s", pwd);
    while (getchar() != '\n') continue;
    

    if (strcmp(pwd, _SECRET_PASSWORD) == 0) {
        read_bot_text("Ой, это ты, прости, не признал. Ну ты же сам понимаешь, система безопасности, все дела.\n"
                "Мало ли кто хотел увидеть, что я на самом деле? Кому бы я тогда голову дурил?\n"
                "Не сердись, вот, держи, я вылил базу в log.html файл.\n"
        );

        Tree base = { };
        int result = read_tree_from_file(&base, DB_FILE);
        if (result != -1) {
            PRINT_WARNING("Cant read base from file. Check correctness of database\n");
            return exit_codes::ERROR;
        }
        LOG_DUMP_GRAPH(&base, "Show base", Tree_dump_graph);
    } else {
        read_bot_text("Агааа!!! Думал надурить мою систему безопасноти? Ты явно не знаешь кто ей занимался!!\n"
                "Пошел прочь отсюда! Не видать тебе ни моих алгоритмов кеширования, ни моей базы, вообще ничего!\n"
                "Радуйся, что я вообще не подчистил тебе систему после такой наглой попытки взлома!!\n"
        );
    }

    return exit_codes::OK;
}

exit_codes get_character_desc() {
    read_bot_text("И на кого же тебе нужно выдать досье, человечишка?\n");

    Tree base = { };
    tree_ctor(&base);
    read_tree_from_file(&base, DB_FILE);

    char* character = (char*) calloc(MAX_ANSWER_SIZE, sizeof(char));
    GET_ANSWER(character);

    std::list<NodeDesc> path = { };
    Node* character_node = find_node_by_value(&base, character, &path, compare_base_values);

    if (character_node == NULL) {
        read_bot_text("Дружище, ты че то попутал. Нет такого кадра в моей базе\n");
        return exit_codes::OK;
    }

    read_bot_text("Ага.. нашел. Вот держи вся инфа по твоему челу.\n");
    print_character_desc(&path);

    read_bot_text("Пссс. Захочешь еще достать инфу по кому-то - обращайся, я люблю сплетничать :)\n");

    return exit_codes::OK;
}

exit_codes print_character_desc(std::list<NodeDesc>* path) {
    ASSERT_IF(VALID_PTR(path), "Invalid path ptr", exit_codes::ERROR);

    printf(GREEN "%s" NATURAL ": ", path->back().node->data);
    path->pop_back();

    for (NodeDesc cur_node_desc : *path) {
        printf("%s%s. ", cur_node_desc.node->data, cur_node_desc.is_left_child ? "" : " (нет)");
    }
    printf("\n");

    return exit_codes::OK;
}

exit_codes compare_characters() {
    read_bot_text("Чьи байты днк кода собраешься сравнивать жалкое подобие живого интеллекта?\n");

    Tree base = { };
    tree_ctor(&base);
    read_tree_from_file(&base, DB_FILE);

    char* first_character = (char*) calloc(MAX_ANSWER_SIZE, sizeof(char));
    GET_ANSWER(first_character);

    std::list<NodeDesc> first_path = { };
    Node* first_character_node = find_node_by_value(&base, first_character, &first_path, compare_base_values);

    if (first_character_node == NULL) {
        read_bot_text("Дружище, ты че то попутал. Нет такого кадра в моей базе\n");
        return exit_codes::OK;
    }

    read_bot_text("Его? Странный ты.. но ладно. С кем сравнивать будешь?\n");
    char* second_character = (char*) calloc(MAX_ANSWER_SIZE, sizeof(char));
    GET_ANSWER(second_character);

    std::list<NodeDesc> second_path = { };
    Node* second_character_node = find_node_by_value(&base, second_character, &second_path, compare_base_values);

    if (second_character_node == NULL) {
        read_bot_text("Дружище, ты че то попутал. Нет такого кадра в моей базе\n");
        return exit_codes::OK;
    }

    print_compare_results(&first_path, &second_path);
    read_bot_text("Доволен тем, что узнал человек? Молчи, знаю, что доволен.\n");

    return exit_codes::OK;
}

exit_codes print_compare_results(std::list<NodeDesc>* first_path, std::list<NodeDesc>* second_path) {
    ASSERT_IF(VALID_PTR(first_path),  "Invalid first_path  ptr", exit_codes::ERROR);
    ASSERT_IF(VALID_PTR(second_path), "Invalid second_path ptr", exit_codes::ERROR);

    Node*  first_node =  first_path->back().node;
    Node* second_node = second_path->back().node;

     first_path->pop_back();
    second_path->pop_back();

    if (strcmp(first_path->front().node->data, second_path->front().node->data) != 0 ||
               first_path->front().is_left_child != second_path->front().is_left_child) {
        printf("'%s' и '%s' ничем не похожи,\n", first_node->data, second_node->data);
    } else {
        printf("'%s' и '%s' похожи тем, что: ",  first_node->data, second_node->data);

        while (strcmp(first_path->front().node->data, second_path->front().node->data) == 0 &&
                      first_path->front().is_left_child == second_path->front().is_left_child) {
            NodeDesc same_node = first_path->front();

             first_path->pop_front();
            second_path->pop_front();

            printf("%s%s. ", same_node.node->data, same_node.is_left_child ? "" : " (нет)");
        }
        printf("\n");
    }

    printf("но, '%s' ", first_node->data);
    for (NodeDesc cur_node_desc : *first_path) {
        printf("%s%s. ", cur_node_desc.node->data, cur_node_desc.is_left_child ? "" : " (нет)");
    }
    printf("\n");

    printf("В то время как '%s' ", second_node->data);
    for (NodeDesc cur_node_desc : *second_path) {
        printf("%s%s. ", cur_node_desc.node->data, cur_node_desc.is_left_child ? "" : " (нет)");
    }
    printf("\n");

    return exit_codes::OK;
}
