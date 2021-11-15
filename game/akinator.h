//
//  Created by IvanBrekman on 14.11.2021.
//

#define GET_ANSWER(ptr) {                       \
    fgets((ptr), MAX_ANSWER_SIZE, stdin);       \
    char* end_index = strchr((ptr), '\n');      \
    if (end_index) *end_index = '\0';           \
}

const int MAX_INPUT_VARIANTS = 10;
const int   MAX_ANSWER_SIZE  = 50;
const char* DB_FILE = "db/base.txt";

struct UserInput {
    const char* possible_input[MAX_INPUT_VARIANTS] = { };
          int   answer_amount = -1;
    // enum answers { a=1, b=2, c=3, d=4 };
};

const UserInput BIN_ANSWERS = {
    { "нет", "да" }, 2
};
const UserInput POSSIBLE_ANSWERS = {
    { "нет", "да", "скорее нет", "скорее да", "не знаю" }, 5
};
const UserInput GAME_MODES = {
    { "Угадай персонажа",
      "Описание персонажа",
      "Сравнение персонажей",
      "База данных",
      "Рассчитать ответ \"В чем смысл жизни?\"",
      "Закончить играть" },
    .answer_amount = 6
};

enum answers {
    NEGATIVE  = 0,
    POSITIVE  = 1,
    MORE_NEG  = 2,
    MORE_POS  = 3,
    UNDEFINED = 4
};
enum game_modes {
    GUESS_CHARACTER        = 0,
    GET_CHARACTER_DESC     = 1,
    COMPARE_TWO_CHARACTERS = 2,
    SHOW_BASE              = 3,
    CALC_LIFE_MEANING      = 4,
    END_PROGRAM            = 5
};

enum exit_codes {
    OK    =   0,
    ERROR = -15
};

int main(int argc, char** argv);

exit_codes read_bot_text(const char* text);
int          get_user_answer(const UserInput user_input, const char* sep="");
exit_codes check_user_asnwer(char* user_answer);
int compare_base_values(node_t val1, node_t val2);

exit_codes guess_character_game();
exit_codes play_guess_character_game(Tree* base);
char* rework_database_with_new_node(Tree* base, std::list<Node*>* end_points);

exit_codes show_base();

exit_codes get_character_desc();
exit_codes print_character_desc(std::list<NodeDesc>* path);

exit_codes compare_characters();
exit_codes print_compare_results(std::list<NodeDesc>* first_path, std::list<NodeDesc>* second_path);
