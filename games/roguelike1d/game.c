#include "game.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

static void gpio_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOB);

    /* Enable GPIOC clock. */
    rcc_periph_clock_enable(RCC_GPIOC);

    /* Set GPIO13 (in GPIO port C) to 'output push-pull'. */
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  GPIO13);
    gpio_set(GPIOC, GPIO13);
}

void game_init_ui(Game_t *game) {
    gpio_setup();

    game->debounce.stable = 0;
    TM1638_Platform_Init(&game->handler);
    TM1638_Init(&game->handler);
    TM1638_ConfigDisplay(&game->handler, 0, TM1638DisplayStateON);

    game->player.health = 5;
    game->player.position = 0;
}

void game_create_map(Game_t *game, uint8_t size) {
    game->map_size = 24;
    for (uint8_t i = 1; i < MAX_MAP_SIZE; ++i)
        game->map_cells[i] = E_FOG;
    game->map_cells[0] = E_PLAYER;

    // hardcoded
    game->map_cells[3] |= E_LOOT;
    game->map_cells[5] |= E_ENEMY;
    game->map_cells[7] |= E_TRAP;
    game->map_cells[10] |= E_ENEMY;
    game->map_cells[14] |= E_LOOT;
    game->map_cells[18] |= E_ENEMY;
    game->map_cells[23] = E_EXIT;
}

void game_render_ui(Game_t *game) {
    uint8_t map_view[8];
    for (uint8_t i = 0; i < 8; ++i) {
        uint8_t map_pos = game->player.position + i;
        if (map_pos < game->map_size) {
            if (game->map_cells[map_pos] & E_FOG)
                map_view[i] = game->map_cells[map_pos]; // E_FOG;
            else
                map_view[i] = game->map_cells[map_pos];
        }
    }
    // map_view[0] |= E_PLAYER;
    TM1638_SetMultipleLEDs(&game->handler, (1 << game->player.health) - 1);
    TM1638_SetMultipleDigit(&game->handler, map_view, 0, 8);
}

void game_message(Game_t *game, const char *text) {
    for (uint8_t i = 0; i < 3; ++i) {
        TM1638_SetMultipleDigit_CHAR(&game->handler, (const uint8_t *)text, 0,
                                     8);
        game->handler.DelayUs(500000);
        TM1638_ClearDigits(&game->handler);
        game->handler.DelayUs(100000);
    }
}

void game_process_input(Game_t *game) {
    uint8_t raw;

    TM1638_ScanKeys(&game->handler, &raw);

    uint8_t prev = game->debounce.stable;
    TM1638_DebounceUpdate(&game->debounce, raw);

    uint8_t pressed = game->debounce.stable & ~prev;

    /* RIGHT key (bit 7, 0x80) */
    if ((pressed & 0x80) && game->player.position < game->map_size)
        game->player.position++;

    /* LEFT key (bit 0, 0x01) */
    if ((pressed & 0x01) && game->player.position > 0)
        game->player.position--;

    if (game->player.position == game->map_size)
        game->player.position = 0;

    game->handler.DelayUs(10000); // 10 ms poll
}
