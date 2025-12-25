/* Simple LED task demo, using timed delays:
 *
 * The LED on PC13 is toggled in task1.
 */
#include "FreeRTOS.h"
#include "task.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,signed portCHAR *pcTaskName);
extern void vApplicationMallocFailedHook(void);

void
vApplicationStackOverflowHook(xTaskHandle *pxTask,signed portCHAR *pcTaskName) {
	(void)pxTask;
	(void)pcTaskName;
	for(;;);
}

/* Called if malloc from FreeRTOS heap fails (configUSE_MALLOC_FAILED_HOOK == 1) */
void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

static void
task1(void *args __attribute((unused))) {

	for (;;) {
		gpio_toggle(GPIOC, GPIO13);
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

static void
task2(void *args __attribute((unused))) {

	for (;;) {
		gpio_toggle(GPIOC, GPIO13);
		vTaskDelay(pdMS_TO_TICKS(174));
	}
}

int
main(void) {
  
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);

	//rcc_periph_clock_enable(RCC_GPIOC);
	//gpio_set_mode(
	//	GPIOC,
	//	GPIO_MODE_OUTPUT_2_MHZ,
	//	GPIO_CNF_OUTPUT_PUSHPULL,
	//	GPIO13);

	xTaskCreate(task1, "LED1", 100, NULL, configMAX_PRIORITIES-1, NULL);
	//xTaskCreate(task2, "LED2", 100, NULL, configMAX_PRIORITIES-1, NULL);
	vTaskStartScheduler();

	for (;;);
	return 0;
}

// End
