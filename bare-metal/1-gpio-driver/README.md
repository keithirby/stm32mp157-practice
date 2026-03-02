# Project 1: GPIO driver 
## Summary 
Create a bare metal application driver from scratch using the STM32MP157 Reference 
manual with **NO** provided HAL (Hardware Abstraction Layer) attached. The goal is 
to make LEDs on the STM32MP157F development board toggleable with some buttons on 
the development board. 

## Files 

1. `gpio.c / gpio.h` 
    - Provides a clean interface for configuring GPIO pins, writing to GPIO pins, and reading GPIO pins