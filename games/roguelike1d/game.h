#include "TM1638_platform.h"
#include "debounce.h"
#include "player.h"
#include <stdint.h>

#define E_ENEMY 0b00000001
#define E_TRAP 0b00000010
#define E_LOOT 0b00000100
#define E_FOG 0b00100000
#define E_PLAYER 0b01000000
#define E_EXIT 0b10111111

#define MAX_MAP_SIZE 50

typedef struct {
    uint8_t map_cells[MAX_MAP_SIZE];
    uint8_t map_size;

    TM1638_Handler_t handler;
    Player_t player;

    TM1638_Debounce_t debounce;
} Game_t;

void game_init_ui(Game_t *game);
void game_create_map(Game_t *game, uint8_t size);

void game_render_ui(Game_t *game);
void game_message(Game_t *game, const char *text);
void game_process_input(Game_t *game);
