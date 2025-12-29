#include "game.h"

int main(void) {
    Game_t game;
    game_init_ui(&game);
    game_message(&game, "rogue-1d");

    game_create_map(&game, 24);

    while (1) {
        game_render_ui(&game);
        game_process_input(&game);
    }
    return 0;
}
