/**
 * startup.s - Cortex-M4 startup code for STM32MP157F
 *
 * References:
 *   PM0214 - STM32 Cortex-M4 Programming Manual
 *     - Section 2.3.4: Vector table, 
 *          - Pg. 40/262
 *     - Section 2.3.2: Exception types (Reset, NMI, HardFault, etc.)
 *          - Pg. 38-39/262
 *   RM0436 - STM32MP157 Reference Manual
 *     - Interrupt vector table for STM32MP1-specific IRQs
 *
 * On reset, Cortex-M4 reads:
 *   - Address 0x00000000 (or VTOR): Initial stack pointer
 *   - Address 0x00000004 (or VTOR+4): Reset handler address
 *
 * This file sets up the vector table and provides a minimal Reset_Handler
 * that zeroes .bss and calls main().
 */

    .syntax unified
    .cpu cortex-m4
    .fpu fpv4-sp-d16
    .thumb

/* -------------------------------------------------------------------------- */
/*  Vector Table                                                              */
/*  Placed in .isr_vector section - linker script puts this first in memory   */
/* -------------------------------------------------------------------------- */

    .section .isr_vector, "a", %progbits
    .type vector_table, %object
vector_table:
    .word _stack_top            /* 0x000: Initial Stack Pointer (from linker) */
    .word Reset_Handler         /* 0x004: Reset */
    .word NMI_Handler           /* 0x008: Non-Maskable Interrupt */
    .word HardFault_Handler     /* 0x00C: Hard Fault */
    .word MemManage_Handler     /* 0x010: Memory Management Fault */
    .word BusFault_Handler      /* 0x014: Bus Fault */
    .word UsageFault_Handler    /* 0x018: Usage Fault */
    .word 0                     /* 0x01C: Reserved */
    .word 0                     /* 0x020: Reserved */
    .word 0                     /* 0x024: Reserved */
    .word 0                     /* 0x028: Reserved */
    .word SVC_Handler           /* 0x02C: SVCall */
    .word DebugMon_Handler      /* 0x030: Debug Monitor */
    .word 0                     /* 0x034: Reserved */
    .word PendSV_Handler        /* 0x038: PendSV */
    .word SysTick_Handler       /* 0x03C: SysTick */

    /*
     * TODO: Add device-specific (peripheral) interrupt vectors here.
     * See RM0436 interrupt and event chapter for the full IRQ list.
     * For now, we only define the core exceptions above.
     * Example:
     *   .word WWDG1_IRQHandler       @ IRQ 0
     *   .word PVD_AVD_IRQHandler     @ IRQ 1
     *   ...
     *
     * For the GPIO driver exercise, we don't need peripheral IRQs yet.
     */

    .size vector_table, . - vector_table

/* -------------------------------------------------------------------------- */
/*  Reset Handler                                                             */
/* -------------------------------------------------------------------------- */

    .section .text
    .type Reset_Handler, %function
    .global Reset_Handler
Reset_Handler:
    /* Step 1: Zero out .bss section */
    ldr r0, =_sbss          /* Start of .bss (from linker script) */
    ldr r1, =_ebss          /* End of .bss */
    movs r2, #0
bss_loop:
    cmp r0, r1
    bge bss_done
    str r2, [r0], #4        /* *r0 = 0; r0 += 4 */
    b bss_loop
bss_done:

    /*
     * Step 2: Copy .data from load address to run address
     * (Not needed when debugger loads directly to SRAM, but included
     * for completeness if you later load from flash)
     *
     * ldr r0, =_sdata       @ destination start
     * ldr r1, =_edata       @ destination end
     * ldr r2, =_etext       @ source (LMA in flash)
     * data_loop:
     *     cmp r0, r1
     *     bge data_done
     *     ldr r3, [r2], #4
     *     str r3, [r0], #4
     *     b data_loop
     * data_done:
     */

    /* Step 3: Enable FPU (Cortex-M4F has an FPU - must enable before use) */
    /* CPACR address: 0xE000ED88 (see PM0214 section 4.6.6) */
    ldr r0, =0xE000ED88
    ldr r1, [r0]
    orr r1, r1, #(0xF << 20)   /* Set CP10 and CP11 to full access */
    str r1, [r0]
    dsb                          /* Data sync barrier */
    isb                          /* Instruction sync barrier */

    /* Step 4: Call main */
    bl main

    /* If main returns, loop forever */
hang:
    b hang

    .size Reset_Handler, . - Reset_Handler

/* -------------------------------------------------------------------------- */
/*  Default Exception Handlers (weak - override in C if needed)               */
/* -------------------------------------------------------------------------- */

    .macro def_irq_handler handler_name
    .weak \handler_name
    .thumb_set \handler_name, Default_Handler
    .endm

    .type Default_Handler, %function
Default_Handler:
    b .                     /* Infinite loop */
    .size Default_Handler, . - Default_Handler

    def_irq_handler NMI_Handler
    def_irq_handler HardFault_Handler
    def_irq_handler MemManage_Handler
    def_irq_handler BusFault_Handler
    def_irq_handler UsageFault_Handler
    def_irq_handler SVC_Handler
    def_irq_handler DebugMon_Handler
    def_irq_handler PendSV_Handler
    def_irq_handler SysTick_Handler

    .end