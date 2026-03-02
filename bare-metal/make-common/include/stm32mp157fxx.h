/**
 * stm32mp157fxx.h - Register definitions for STM32MP157F
 *
 * Reference: RM0436 - STM32MP157 Reference Manual
 *
 * This file defines base addresses and register structures for peripherals
 * used in the bare metal exercises. It is NOT a complete device header -
 * add registers as you need them.
 *
 * IMPORTANT: All register access MUST use volatile pointers. The compiler
 * will otherwise optimize away reads/writes to hardware registers.
 *
 * How to use this file:
 *   1. Find the peripheral base address in RM0436 memory map
 *   2. Find register offsets in the peripheral's chapter
 *   3. Define them here as a struct or as base + offset macros
 *
 */

#ifndef STM32MP157FXX_H
#define STM32MP157FXX_H

#include <stdint.h>

/* ==========================================================================  */
/*  Peripheral Base Addresses                                                  */
/*  Reference: RM0436 - Memory Map  Pg. 164/4054                               */
/* ==========================================================================  */

/* AHB4 peripherals (GPIO ports live here) */
#define GPIOA_BASE      0x50002000UL // 0x5000_2000 - 0x5000_23FF
#define GPIOB_BASE      0x50003000UL // 0x5000_3000 - 0x5000_33FF
#define GPIOC_BASE      0x50004000UL // 0x5000_4000 - 0x5000_43FF
#define GPIOD_BASE      0x50005000UL // 0x5000_5000 - 0x5000_53FF
#define GPIOE_BASE      0x50006000UL // 0x5000_6000 - 0x5000_63FF
#define GPIOF_BASE      0x50007000UL // 0x5000_7000 - 0x5000_73FF
#define GPIOG_BASE      0x50008000UL // 0x5000_8000 - 0x5000_83FF
#define GPIOH_BASE      0x50009000UL // 0x5000_9000 - 0x5000_93FF
#define GPIOI_BASE      0x5000A000UL // 0x5000_A000 - 0x5000_A3FF
#define GPIOJ_BASE      0x5000B000UL // 0x5000_B000 - 0x5000_B3FF
#define GPIOK_BASE      0x5000C000UL // 0x5000_C000 - 0x5000_C3FF

/* RCC (Reset and Clock Control) */
#define RCC_BASE        0x50000000UL // 0x5000_0000 - 0x5000_0FFF (4KB)

/* ========================================================================== */
/*  GPIO Registers                                                             */
/*  Reference: RM0436 - GPIO chapter                                           */
/*                                                                             */
/*  Each GPIO port has the same register layout. Key registers:                */
/*    MODER   - Pin mode (input/output/alternate/analog), 2 bits per pin       */
/*    OTYPER  - Output type (push-pull/open-drain), 1 bit per pin              */
/*    OSPEEDR - Output speed, 2 bits per pin                                   */
/*    PUPDR   - Pull-up/pull-down, 2 bits per pin                              */
/*    IDR     - Input data (read pin state), 1 bit per pin                     */
/*    ODR     - Output data, 1 bit per pin                                     */
/*    BSRR    - Bit set/reset (atomic set/clear of output pins)                */
/*              Bits [15:0] = SET, Bits [31:16] = RESET                        */
/*    AFRL/H  - Alternate function selection, 4 bits per pin                   */
/* ========================================================================== */

typedef struct {
    volatile uint32_t MODER;        /* 0x00 - Mode register Pg. 1078/4054 */
    volatile uint32_t OTYPER;       /* 0x04 - Output type register Pg. 1078/4054 */
    volatile uint32_t OSPEEDR;      /* 0x08 - Output speed register Pg. 1079/4054 */
    volatile uint32_t PUPDR;        /* 0x0C - Pull-up/pull-down register Pg. 1079/4054 */
    volatile uint32_t IDR;          /* 0x10 - Input data register Pg. 1079/4054 */
    volatile uint32_t ODR;          /* 0x14 - Output data register Pg. 1080/4054*/
    volatile uint32_t BSRR;         /* 0x18 - Bit set/reset register Pg. 1080/4054 */
    volatile uint32_t LCKR;         /* 0x1C - Lock register Pg. 1081/4054 */
    volatile uint32_t AFRL;         /* 0x20 - Alternate function low (pins 0-7) Pg. 1082/4054 */
    volatile uint32_t AFRH;         /* 0x24 - Alternate function high (pins 8-15) Pg. 1083/4054 */
} GPIO_TypeDef;

/* ==========================================================================  */
/*  RCC Registers (partial - clock enable bits for GPIO)                       */
/*  Reference: RM0436 - RCC chapter                                            */
/*                                                                             */
/*  The STM32MP157 has a complex RCC with separate registers for MP            */
/*  (Cortex-A7) and MCU (Cortex-M4) clock domains. For M4 bare metal,          */
/*  GPIO clocks are on AHB4 and enabled via RCC_MC_AHB4ENSETR.                 */
/*                                                                             */
/*  TODO: Add more RCC registers as you need them for UART, SPI, etc later.    */
/* ==========================================================================  */

typedef struct {
    /* This is a PARTIAL definition - only the registers we need right now.
     * The RCC register block is very large. We use padding to reach the
     * offsets we care about.
     *
     * Address offset: 0xAA8 found in RM0436 Pg. 873/4054
     */

    volatile uint32_t _reserved0[((0xAA8 - 0x000) / 4)];

    /* RCC_MC_AHB4ENSETR - MCU AHB4 peripheral clock enable set register
     * Reference: RM0436 RCC chapter
     *
     * Bit 0:  GPIOAEN - GPIOA clock enable
     * Bit 1:  GPIOBEN - GPIOB clock enable
     * Bit 2:  GPIOCEN - GPIOC clock enable
     * Bit 3:  GPIODEN - GPIOD clock enable
     * Bit 4:  GPIOEEN - GPIOE clock enable
     * Bit 5:  GPIOFEN - GPIOF clock enable
     * Bit 6:  GPIOGEN - GPIOG clock enable
     * Bit 7:  GPIOHEN - GPIOH clock enable
     * Bit 8:  GPIOIEN - GPIOI clock enable
     * Bit 9:  GPIOJEN - GPIOJ clock enable
     * Bit 10: GPIOKEN - GPIOK clock enable
     *
     * Write 1 to set (enable). Reading returns current state.
     */
    volatile uint32_t MC_AHB4ENSETR;  /* Offset: 0xAA8*/

} RCC_TypeDef;

/* ========================================================================== */
/*  Peripheral Pointer Macros                                                  */
/*  Cast base addresses to struct pointers for register access                 */
/* ========================================================================== */

#define GPIOA   ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB   ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC   ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD   ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE   ((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOF   ((GPIO_TypeDef *) GPIOF_BASE)
#define GPIOG   ((GPIO_TypeDef *) GPIOG_BASE)
#define GPIOH   ((GPIO_TypeDef *) GPIOH_BASE)
#define GPIOI   ((GPIO_TypeDef *) GPIOI_BASE)
#define GPIOJ   ((GPIO_TypeDef *) GPIOJ_BASE)
#define GPIOK   ((GPIO_TypeDef *) GPIOK_BASE)

#define RCC     ((RCC_TypeDef *) RCC_BASE)

/* ========================================================================== */
/*  Bit Manipulation Helpers                                                   */
/* ========================================================================== */

/* GPIO MODER values (2 bits per pin) */
#define GPIO_MODE_INPUT     0x00U
#define GPIO_MODE_OUTPUT    0x01U
#define GPIO_MODE_AF        0x02U   /* Alternate function */
#define GPIO_MODE_ANALOG    0x03U

/* GPIO OTYPER values (1 bit per pin) */
#define GPIO_OTYPE_PP       0x00U   /* Push-pull */
#define GPIO_OTYPE_OD       0x01U   /* Open-drain */

/* GPIO OSPEEDR values (2 bits per pin) */
#define GPIO_SPEED_LOW      0x00U
#define GPIO_SPEED_MED      0x01U
#define GPIO_SPEED_HIGH     0x02U
#define GPIO_SPEED_VHIGH    0x03U

/* GPIO PUPDR values (2 bits per pin) */
#define GPIO_PUPD_NONE      0x00U
#define GPIO_PUPD_UP        0x01U
#define GPIO_PUPD_DOWN      0x02U

/* RCC AHB4 enable bits for GPIO ports */
#define RCC_AHB4_GPIOAEN   (1U << 0)
#define RCC_AHB4_GPIOBEN   (1U << 1)
#define RCC_AHB4_GPIOCEN   (1U << 2)
#define RCC_AHB4_GPIODEN   (1U << 3)
#define RCC_AHB4_GPIOEEN   (1U << 4)
#define RCC_AHB4_GPIOFEN   (1U << 5)
#define RCC_AHB4_GPIOGEN   (1U << 6)
#define RCC_AHB4_GPIOHEN   (1U << 7)
#define RCC_AHB4_GPIOIEN   (1U << 8)
#define RCC_AHB4_GPIOJEN   (1U << 9)
#define RCC_AHB4_GPIOKEN   (1U << 10)

#endif /* STM32MP157FXX_H */