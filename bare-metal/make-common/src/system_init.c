/**
 * system_init.c - System initialization for STM32MP157F Cortex-M4
 *
 * Reference: RM0436 - RCC chapter
 *
 * This file handles early system setup that all exercises share:
 *   - Enable peripheral clocks
 *   - (Optional) Configure NVIC, SysTick
 *
 * IMPORTANT NOTE on the STM32MP157 dual-core architecture:
 *
 * The Cortex-A7 cores handle the main system clock tree (PLLs, HSE, etc.).
 * When the M4 is started via Linux remoteproc, the A7 side has already
 * configured the clocks. Our job is mainly to enable the peripheral clocks
 * on the AHB/APB buses we need.
 *
 * If running fully standalone (no Linux on A7), you'd need to configure
 * the full clock tree here. That's an advanced exercise.
 */

#include "stm32mp157fxx.h"

/**
 * system_init() - Perform basic system initialization
 *
 * Call this at the start of main() before using any peripherals.
 */
void system_init(void)
{
    /*
     * Enable GPIO clocks on AHB4
     *
     * The RCC_MC_AHB4ENSETR register is a "set" register:
     * writing a 1 enables the clock, writing a 0 has no effect.
     * This is an atomic set operation - no read-modify-write needed.
     *
     * Enable the GPIO ports you need. For the GPIO driver exercise,
     * you'll need at least the port(s) connected to the LEDs and button.
     *
     * TODO: Check the DK2 schematic (MB1272) to find which GPIO ports
     * the LEDs and user button are on, then enable only those.
     * For now, we enable GPIOA and GPIOH as likely candidates.
     */
    RCC->MC_AHB4ENSETR |= RCC_AHB4_GPIOAEN;   /* Enable GPIOA clock */
    RCC->MC_AHB4ENSETR |= RCC_AHB4_GPIOHEN;   /* Enable GPIOH clock */

    /*
     * Small delay after enabling clocks.
     * The peripheral needs a couple of clock cycles after the enable bit
     * is set before you can access its registers. A read-back of the
     * enable register is a common idiom to ensure the write has completed.
     */
    volatile uint32_t dummy;
    dummy = RCC->MC_AHB4ENSETR;
    (void)dummy;

    /*
     * TODO (future exercises):
     * - Enable UART clock on APB1 for exercise 2
     * - Enable SPI clock for exercise 3
     * - Configure SysTick for delay functions
     */
}

/**
 * delay_cycles() - Crude busy-wait delay
 *
 * NOT accurate, but useful for blinking LEDs before you have a timer.
 * Each loop iteration is roughly 4 cycles on Cortex-M4.
 *
 * For a ~1 second delay at 209 MHz (typical M4 clock from A7 config):
 *   delay_cycles(209000000 / 4) ≈ 52250000
 *
 * TODO: Replace with SysTick-based delay for accuracy.
 */
void delay_cycles(volatile uint32_t count)
{
    while (count--) {
        __asm volatile ("nop");
    }
}