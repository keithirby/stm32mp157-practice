/*
 * linker.ld - Linker script for STM32MP157F Cortex-M4 bare metal
 *
 * Memory Map (from RM0436 Pg. 160/4054):
 *   MCU SRAM1: 0x1000_0000 - 0x1001_FFFF (128KB)
 *   MCU SRAM2: 0x1002_0000 - 0x1003_FFFF (128KB)
 *   MCU SRAM3: 0x1004_0000 - 0x1005_FFFF (128KB)
 *   MCU SRAM3: 0x1004_0000 - 0x1004_FFFF (64KB)
 *   MCU SRAM4: 0x1005_0000 - 0x1005_FFFF (64KB) 
 *   RETRAM:    0x0000_0000 - 0x0000_FFFF (64KB)
 *      - RETRAM (Retention RAM), low power internal SRAM used to store retention firmware
 *
 * We place the vector table and code in SRAM1, and use SRAM2 for data/stack.
 * Adjust sizes based on your needs and what RM0436 confirms.
 */

MEMORY
{
    /* Main code memory - MCU SRAM1 */
    SRAM1 (rwx) : ORIGIN = 0x10000000, LENGTH = 128K

    /* Data and stack - MCU SRAM2 */
    SRAM2 (rwx) : ORIGIN = 0x10020000, LENGTH = 128K
}

/* Top of stack = end of SRAM2 */
_stack_top = ORIGIN(SRAM2) + LENGTH(SRAM2);

ENTRY(Reset_Handler)

SECTIONS
{
    /* --- Code and read-only data in SRAM1 --- */

    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))   /* Vector table must be first */
        . = ALIGN(4);
    } > SRAM1

    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;            /* End of text - used to copy .data from flash */
    } > SRAM1

    /* --- Initialized data --- */
    /*
     * Since we're running entirely from SRAM (loaded by debugger or remoteproc),
     * .data can live in SRAM2. If loading from flash, you'd need a LMA in flash
     * and copy .data to RAM in startup code.
     */

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > SRAM2

    /* --- Zero-initialized data --- */

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM2

    /* Ensure the stack doesn't collide with .bss */
    ._stack_check :
    {
        . = ALIGN(8);
        . = . + 2K;           /* Minimum 2KB stack - increase as needed */
        . = ALIGN(8);
    } > SRAM2
}
