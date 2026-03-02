/**
 * gpio.h - GPIO driver interface for STM32MP157F
 *
 * Reference: RM0436 - GPIO chapter
 *
 * This driver provides a clean interface for:
 *   - Configuring GPIO pins (mode, speed, pull-up/down, output type)
 *   - Writing output pins (set, clear, toggle)
 *   - Reading input pins
 *
 * Design notes:
 *   - Uses a configuration struct so pin setup is declarative
 *   - All functions take a GPIO_TypeDef* port pointer and pin number
 *   - No dynamic memory allocation
 */

#ifndef GPIO_H
#define GPIO_H

#include "stm32mp157fxx.h"

/* ========================================================================== */
/*  GPIO Pin Configuration                                                     */
/* ========================================================================== */

/**
 * struct gpio_config - Configuration for a single GPIO pin
 * @mode:    GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG
 * @otype:   GPIO_OTYPE_PP (push-pull) or GPIO_OTYPE_OD (open-drain)
 * @speed:   GPIO_SPEED_LOW / MED / HIGH / VHIGH
 * @pull:    GPIO_PUPD_NONE / GPIO_PUPD_UP / GPIO_PUPD_DOWN
 * @af:      Alternate function number (0-15), only used when mode == GPIO_MODE_AF
 */
typedef struct {
    uint32_t mode;
    uint32_t otype;
    uint32_t speed;
    uint32_t pull;
    uint32_t af;
} gpio_config_t;

/* ========================================================================== */
/*  Function Prototypes                                                        */
/* ========================================================================== */

/**
 * gpio_init() - Configure a GPIO pin
 * @port: Pointer to GPIO port (e.g., GPIOA, GPIOH)
 * @pin:  Pin number (0-15)
 * @cfg:  Pointer to configuration struct
 *
 * Configures MODER, OTYPER, OSPEEDR, PUPDR, and (if AF mode) AFR registers.
 */
void gpio_init(GPIO_TypeDef *port, uint8_t pin, const gpio_config_t *cfg);

/**
 * gpio_write() - Set a pin HIGH or LOW
 * @port:  Pointer to GPIO port
 * @pin:   Pin number (0-15)
 * @value: 0 = LOW, non-zero = HIGH
 *
 * Uses BSRR for atomic set/reset (no read-modify-write race condition).
 */
void gpio_write(GPIO_TypeDef *port, uint8_t pin, uint8_t value);

/**
 * gpio_toggle() - Toggle a pin's output state
 * @port: Pointer to GPIO port
 * @pin:  Pin number (0-15)
 *
 * Reads ODR and flips the bit via BSRR. Note: this is NOT atomic if
 * an interrupt could modify the same port between the read and write.
 */
void gpio_toggle(GPIO_TypeDef *port, uint8_t pin);

/**
 * gpio_read() - Read the current state of an input pin
 * @port: Pointer to GPIO port
 * @pin:  Pin number (0-15)
 *
 * Returns: 0 if LOW, 1 if HIGH (reads from IDR)
 */
uint8_t gpio_read(GPIO_TypeDef *port, uint8_t pin);

#endif /* GPIO_H */