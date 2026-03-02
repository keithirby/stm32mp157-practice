/**
 * gpio.c - GPIO driver implementation for STM32MP157F
 *
 * Reference: RM0436 - GPIO chapter
 *
 * Register access pattern for 2-bit-per-pin registers (MODER, OSPEEDR, PUPDR):
 *   1. Clear the 2-bit field:  reg &= ~(0x3 << (pin * 2))
 *   2. Set the new value:      reg |=  (value << (pin * 2))
 *
 * Register access pattern for 1-bit-per-pin registers (OTYPER):
 *   1. Clear the bit:   reg &= ~(1 << pin)
 *   2. Set if needed:   reg |=  (value << pin)
 *
 * Register access pattern for BSRR (set/reset, write-only):
 *   - To SET pin N:     BSRR = (1 << N)       (bits [15:0])
 *   - To RESET pin N:   BSRR = (1 << (N+16))  (bits [31:16])
 *
 * Register access pattern for AFR (4-bit-per-pin, split into AFRL/AFRH):
 *   - Pins 0-7  use AFRL:  AFRL &= ~(0xF << (pin * 4)); AFRL |= (af << (pin * 4))
 *   - Pins 8-15 use AFRH:  AFRH &= ~(0xF << ((pin-8) * 4)); AFRH |= (af << ((pin-8) * 4))
 */

#include "gpio.h"

void gpio_init(GPIO_TypeDef *port, uint8_t pin, const gpio_config_t *cfg)
{
    uint32_t temp;

    /* --- Configure MODER (2 bits per pin) --- */
    temp = port->MODER;
    temp &= ~(0x3UL << (pin * 2));         /* Clear existing mode */
    temp |=  (cfg->mode << (pin * 2));      /* Set new mode */
    port->MODER = temp;

    /* --- Configure OTYPER (1 bit per pin) --- */
    temp = port->OTYPER;
    temp &= ~(0x1UL << pin);               /* Clear existing type */
    temp |=  (cfg->otype << pin);           /* Set new type */
    port->OTYPER = temp;

    /* --- Configure OSPEEDR (2 bits per pin) --- */
    temp = port->OSPEEDR;
    temp &= ~(0x3UL << (pin * 2));
    temp |=  (cfg->speed << (pin * 2));
    port->OSPEEDR = temp;

    /* --- Configure PUPDR (2 bits per pin) --- */
    temp = port->PUPDR;
    temp &= ~(0x3UL << (pin * 2));
    temp |=  (cfg->pull << (pin * 2));
    port->PUPDR = temp;

    /* --- Configure Alternate Function (if AF mode) --- */
    if (cfg->mode == GPIO_MODE_AF) {
        if (pin < 8) {
            /* AFRL covers pins 0-7 (4 bits per pin) */
            temp = port->AFRL;
            temp &= ~(0xFUL << (pin * 4));
            temp |=  (cfg->af << (pin * 4));
            port->AFRL = temp;
        } else {
            /* AFRH covers pins 8-15 (4 bits per pin) */
            temp = port->AFRH;
            temp &= ~(0xFUL << ((pin - 8) * 4));
            temp |=  (cfg->af << ((pin - 8) * 4));
            port->AFRH = temp;
        }
    }
}

void gpio_write(GPIO_TypeDef *port, uint8_t pin, uint8_t value)
{
    /*
     * BSRR is write-only and atomic:
     *   Writing to bits [15:0]  SETS the corresponding ODR bit
     *   Writing to bits [31:16] RESETS the corresponding ODR bit
     *
     * This avoids the read-modify-write race that ODR has.
     */
    if (value) {
        port->BSRR = (1UL << pin);           /* Set pin HIGH */
    } else {
        port->BSRR = (1UL << (pin + 16));    /* Set pin LOW */
    }
}

void gpio_toggle(GPIO_TypeDef *port, uint8_t pin)
{
    /*
     * Read current output state from ODR, then use BSRR to flip it.
     *
     * WARNING: This is NOT fully atomic. If an ISR modifies a different
     * pin on the same port between the ODR read and the BSRR write,
     * it's fine (BSRR is per-bit). But if an ISR toggles the SAME pin,
     * you could get unexpected behavior. For ISR-safe toggling of the
     * same pin, disable interrupts around this operation.
     */
    if (port->ODR & (1UL << pin)) {
        port->BSRR = (1UL << (pin + 16));    /* Was HIGH, set LOW */
    } else {
        port->BSRR = (1UL << pin);           /* Was LOW, set HIGH */
    }
}

uint8_t gpio_read(GPIO_TypeDef *port, uint8_t pin)
{
    /*
     * IDR is read-only and reflects the actual pin state.
     * Return 0 or 1.
     */
    return (port->IDR >> pin) & 0x01U;
}