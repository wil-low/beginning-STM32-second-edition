.syntax unified
.cpu cortex-m3
.thumb

.global Reset_Handler
.global Default_Handler
.global _estack

_estack = 0x20005000  /* end of 20 KB SRAM */

.section .isr_vector,"a",%progbits
.word _estack
.word Reset_Handler
.word Default_Handler  /* NMI_Handler */
.word Default_Handler  /* HardFault_Handler */
.word Default_Handler  /* MemManage_Handler */
.word Default_Handler  /* BusFault_Handler */
.word Default_Handler  /* UsageFault_Handler */
.word 0
.word 0
.word 0
.word 0
.word Default_Handler  /* SVC_Handler */
.word Default_Handler  /* DebugMon_Handler */
.word 0
.word Default_Handler  /* PendSV_Handler */
.word Default_Handler  /* SysTick_Handler */

Reset_Handler:
    /* Copy .data section from Flash to SRAM */
    LDR R0, =__data_start__
    LDR R1, =__data_end__
    LDR R2, =_lm_rom_img_cfgp   /* source in flash */

copy_data:
    CMP R0, R1
    IT LT
    LDRLT R3, [R2], #4
    IT LT
    STRLT R3, [R0], #4
    BLT copy_data

    /* Zero .bss section */
    LDR R0, =__bss_start__
    LDR R1, =__bss_end__

zero_bss:
    CMP R0, R1
    IT LT
    MOVLT R3, #0
    IT LT
    STRLT R3, [R0], #4
    BLT zero_bss

    /* Set VTOR to application start (optional) */
    LDR R0, =0xE000ED08
    LDR R1, =0x08002000
    STR R1, [R0]

    /* Call main() */
    BL main

    /* If main returns, loop forever */
hang:
    B hang

Default_Handler:
    B .
