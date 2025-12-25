#include "TM1638_platform.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

static void gpio_setup(void) {

    rcc_periph_clock_enable(RCC_GPIOB);

    /* Enable GPIOC clock. */
    rcc_periph_clock_enable(RCC_GPIOC);

    /* Set GPIO13 (in GPIO port C) to 'output push-pull'. */
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  GPIO13);
    gpio_set(GPIOC, GPIO13);
}

int main(void) {
    gpio_setup();

    TM1638_Handler_t Handler = {0};

    TM1638_Platform_Init(&Handler);
    TM1638_Init(&Handler);
    TM1638_ConfigDisplay(&Handler, 0, TM1638DisplayStateON);
    /*
        for (;;) {
            gpio_set(GPIOC, GPIO13);
            Handler.DelayUs(1000000);
            gpio_clear(GPIOC, GPIO13);
            Handler.DelayUs(1000000);
        }
    */

    for (;;) {
        for (uint16_t i = 0; i < 10000; i++) {
            uint8_t Buffer[4] = {0};
            Buffer[3] = i % 10;
            Buffer[2] = (i / 10) % 10;
            Buffer[1] = (i / 100) % 10;
            Buffer[0] = (i / 1000) % 10;
            Buffer[1] |= TM1638DecimalPoint;
            TM1638_SetMultipleLEDs(&Handler, i / 10);
            TM1638_SetMultipleDigit_HEX(&Handler, Buffer, 2, 4);
            Handler.DelayUs(1);
        }
    }
    TM1638_DeInit(&Handler);

    return 0;
}
