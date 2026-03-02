# `make-common/` Directory  

### Summary 

The `make-common/` directory holds is a directory for holding all necessary 
startup code for bare metal applications. This includes: 

1. `linker.ld`  : linker script 
    - Configuration file that tells the linker tool exactly where to map my code and data 
      onto physical memory of the hardware.
    - Manual blueprint of the device memory layout for my program.
2. `startup.s`  : startup file 
    - Assembly code file that provides low-level initialization 
      for out program / micro controller that is needed before 
      the main program can run. 
3. `src/system_init.c` : system initialization file
    - TODO
4. `/include/stm32mp157fxx.h` : Header file definitions for the stm32p32mp157 processor
    - TODO
5. `Makefile` : Make 



# Projects


### Project 1: GPIO driver 

Location: `1-gpio-driver/`

#### Summary 
Create a bare metal application driver from scratch using the STM32MP157 Reference manual with **NO** provided HAL (Hardware Abstraction Layer) attached. The goal is to make LEDs on the STM32MP157F development board toggleable with some buttons on the development board. 