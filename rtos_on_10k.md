Good — thanks for the config file and the SRAM size. You can absolutely run FreeRTOS on a Blue Pill with **10 KB** SRAM, but you must tune the FreeRTOS configuration so the kernel, stacks, heap and your `.data/.bss` fit inside physical RAM.

Below I’ll:

1. explain the important knobs you should adjust for a 10 KB device,
2. give a **recommended** `FreeRTOSConfig.h` tuned for 10 KB SRAM, and
3. list runtime checks and tips to verify memory usage.

---

## Key points to understand before changing values

* RAM on the chip is **fixed** (you said 10 KB). All of these must live inside it:

  * `.data` (initialized globals)
  * `.bss` (uninitialized globals)
  * The **stack** (main stack and any ISR stacks)
  * FreeRTOS **heap** (if you use dynamic allocation for task stacks)
  * **Task stacks** (each task uses its own stack)
* Keep number of tasks small and keep each task stack small. Use `vTaskDelay()` rather than busy loops to let the CPU sleep between ticks.
* `configCPU_CLOCK_HZ` must match your actual CPU clock (e.g. `72000000UL` if you use HSE8+PLL).
* `configSYSTICK_CLOCK_HZ` should reflect the SysTick clock source (HCLK or HCLK/8). If you use the default Cortex-M SysTick with processor clock, set it to `configCPU_CLOCK_HZ`. The `configSYSTICK_CLOCK_HZ = configCPU_CLOCK_HZ/8` is only correct if SysTick is clocked at `HCLK/8`.
* Keep `configTOTAL_HEAP_SIZE` small but large enough for the sum of all task stacks + allocations. For 10 KB, **1–4 KB** is typical depending on how many tasks you create.
* Enable `configCHECK_FOR_STACK_OVERFLOW` and `configASSERT` to catch issues early.
* Use static allocation (`heap_1.c` or `heap_2.c`) or `heap_4.c` with a small heap. `heap_4.c` is flexible; `heap_1.c` is simplest but no freeing.

---

## Suggested `FreeRTOSConfig.h` for 10 KB SRAM (practical, conservative)

This config assumes one or two small tasks (main task + maybe one extra), small stacks, and 72 MHz CPU. Adjust values if you use different clock or more tasks.

```c
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdint.h>

/* Basic scheduler features */
#define configUSE_PREEMPTION            1
#define configUSE_IDLE_HOOK             0
#define configUSE_TICK_HOOK             0

/* CPU clock — set to your actual CPU clock */
#define configCPU_CLOCK_HZ              ( ( unsigned long ) 72000000UL )

/* SysTick clock source:
   If SysTick is clocked at HCLK (recommended), set SYSTICK = CPU clock.
   If you configured SysTick to HCLK/8, change accordingly.
*/
#define configSYSTICK_CLOCK_HZ          ( configCPU_CLOCK_HZ )

/* Tick rate (ms) */
#define configTICK_RATE_HZ              ( ( TickType_t ) 1000 )  /* 1 ms tick */

/* Task and heap sizing */
#define configMINIMAL_STACK_SIZE        ( ( unsigned short ) 128 )  /* words (not bytes) — adjust if tasks crash */
#define configTOTAL_HEAP_SIZE           ( ( size_t ) ( 2048 ) )     /* 2 KB heap (tune between 1024..4096) */

#define configMAX_PRIORITIES            ( 4 )
#define configMAX_TASK_NAME_LEN         ( 12 )

/* Enable stack overflow checking (preferred mode 2) */
#define configCHECK_FOR_STACK_OVERFLOW  2

/* Useful debugging hooks */
#define configUSE_MALLOC_FAILED_HOOK    1
#define configUSE_TRACE_FACILITY        0
#define configUSE_16_BIT_TICKS          0
#define configIDLE_SHOULD_YIELD         1
#define configUSE_MUTEXES               1

/* Co-routines (not used) */
#define configUSE_CO_ROUTINES           0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Optional API functions to be included */
#define INCLUDE_vTaskPrioritySet        0
#define INCLUDE_uxTaskPriorityGet       0
#define INCLUDE_vTaskDelete             0
#define INCLUDE_vTaskCleanUpResources   0
#define INCLUDE_vTaskSuspend            0
#define INCLUDE_vTaskDelayUntil         0
#define INCLUDE_vTaskDelay              1

/* Cortex-M3 NVIC priority configuration (do not change lightly) */
#define configKERNEL_INTERRUPT_PRIORITY         255
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    191  /* 0xb0, priority 11 */
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY 15   /* 15 maps to lowest priority (255) */

#endif /* FREERTOS_CONFIG_H */
```

### Why these values?

* `configTOTAL_HEAP_SIZE = 2048` (2 KB) is a compromise: leaves room for `.data/.bss` + stacks. If your program uses few tasks, you can lower it to 1 KB; if you need more tasks, increase but watch RAM.
* `configMINIMAL_STACK_SIZE = 128` (words) → ~512 bytes per stack on 32-bit system. For most simple tasks on Blue Pill this is OK; increase to 160–192 if you use printf(), large local arrays, or floating operations.
* `configTICK_RATE_HZ = 1000` gives millisecond granularity. You can choose 250 Hz to reduce tick overhead, but with 1 ms you can use `vTaskDelay(pdMS_TO_TICKS(x))`.
* `configCHECK_FOR_STACK_OVERFLOW = 2` is the strongest check (needs port support) and will generate a callback on overflow.

---

## Hooks you must implement (if enabled)

Add these functions to your project (they help find problems quickly):

```c
/* Called if a stack overflow is detected (configCHECK_FOR_STACK_OVERFLOW != 0) */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask; (void)pcTaskName;
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

/* Called if malloc from FreeRTOS heap fails (configUSE_MALLOC_FAILED_HOOK == 1) */
void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    for( ;; );
}
```

Also include the SysTick bridge (we covered earlier):

```c
/* Required: call FreeRTOS tick handler from SysTick ISR */
void sys_tick_handler(void)
{
    xPortSysTickHandler();
}
```

---

## Tips to save RAM and ensure success

1. **Use very few tasks.** Start with one task (the blink task). Add more only after checking memory usage.
2. **Use `vTaskDelay()`** instead of busy loops — that reduces CPU use and avoids large stack usage in tasks.
3. **Choose smaller stack sizes** per task, but test with `configCHECK_FOR_STACK_OVERFLOW=2`. If hook fires, increase that task’s stack.
4. **Prefer static allocation** for critical tasks (`xTaskCreateStatic`) to avoid heap pressure.
5. **Switch to `heap_1.c` or `heap_2.c`** if you never need free() — `heap_1.c` is smallest. If you need malloc/free, use `heap_4.c`.
6. **Minimize global/static buffers** (big arrays) — move large buffers to external RAM or reduce them.
7. **Measure actual .data/.bss size**: `arm-none-eabi-size firmware.elf` and inspect `.data` and `.bss`. Sum `.data + .bss + configTOTAL_HEAP_SIZE + (sum of stacks)` must be < physical RAM minus some safety margin.
8. **Enable `configASSERT`** to catch invalid API use early:

```c
#define configASSERT(x) if((x) == 0) { taskDISABLE_INTERRUPTS(); for(;;); }
```

---

## Example memory budgeting (rough for 10 KB)

Assume:

* `.data + .bss` ≈ 1 KB (depends on globals)
* `configTOTAL_HEAP_SIZE` = 2 KB
* Idle task stack ~ 256 bytes (system)
* One user task stack = 512 bytes

Total RAM usage ≈ 1 KB + 2 KB + 0.25 KB + 0.5 KB = 3.75 KB → fits into 10 KB with plenty of margin.

If you create more tasks or larger stacks, increase heap accordingly or move to a chip with more RAM.

---

## Debug checklist if system still hangs

1. Confirm `sys_tick_handler()` exists and is linked.
2. Check CPU clock value is correct for `configCPU_CLOCK_HZ`. Wrong CPU_CLOCK → wrong SysTick period → scheduler misbehaves.
3. Step into `vTaskStartScheduler()` with a debugger to see if the Idle task gets created.
4. If using semihosting/printf in tasks, that can inflate stack usage — avoid in small RAM targets.
5. If heap fails, `vApplicationMallocFailedHook()` will catch it (ensure enabled).
6. Increase `configMINIMAL_STACK_SIZE` temporarily to 192 to rule out stack overflow.
7. Inspect `.map` or use `arm-none-eabi-size` to verify .data/.bss sizes.

---

## Want me to do this for you?

I can:

* produce a **complete minimal FreeRTOS project** for STM32F103C6 (libopencm3) with a tested `Makefile`, `FreeRTOSConfig.h`, `sys_tick_handler()`, and a blinking task that fits into 10 KB; or
* produce a **memory report** for your current ELF (showing .data, .bss, .text sizes) and recommend exact heap & stack settings.

Which do you prefer?
