# ==============================================================================
# Makefile - STM32MP157F-DK2 Bare Metal Exercises
#
# Builds all exercises from a single Makefile. Each exercise directory
# (1-gpio-driver, 2-uart-driver, etc.) is auto-discovered and compiled
# against the shared common/ code.
#
# Usage:
#   make                    Build all exercises
#   make 1-gpio-driver      Build a single exercise
#   make flash-1-gpio-driver  Build and flash via OpenOCD
#   make debug-1-gpio-driver  Start GDB debug session
#   make clean              Remove all build artifacts
# ==============================================================================

# --- Toolchain ---
CC      = arm-none-eabi-gcc
AS      = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE    = arm-none-eabi-size
GDB     = gdb-multiarch

# --- Directories ---
BUILD_DIR  = build
COMMON_DIR = common

# --- Cortex-M4 flags ---
CPU_FLAGS = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

CFLAGS  = $(CPU_FLAGS) \
          -nostdlib -ffreestanding \
          -Wall -Wextra -Wpedantic \
          -std=c11 \
          -O2 -g \
          -I$(COMMON_DIR)/include

ASFLAGS = $(CPU_FLAGS)

LDFLAGS = -T $(COMMON_DIR)/linker.ld \
          -nostdlib \
          -Wl,-Map=$(BUILD_DIR)/$*/firmware.map \
          -Wl,--gc-sections

# --- Auto-discover exercise directories ---
EXERCISES := $(sort $(wildcard [0-9]*-*))

# --- Common source files ---
COMMON_C_SRC   := $(wildcard $(COMMON_DIR)/src/*.c)
COMMON_C_OBJ   := $(patsubst $(COMMON_DIR)/%.c, $(BUILD_DIR)/common/%.o, $(COMMON_C_SRC))
STARTUP_OBJ    := $(BUILD_DIR)/common/startup.o

# --- OpenOCD config ---
OPENOCD_CFG = board/stm32mp15x_dk2.cfg
M4_LOAD_ADDR = 0x10000000

# ==============================================================================
# Targets
# ==============================================================================

.PHONY: all clean $(EXERCISES) help

all: $(EXERCISES)

help:
	@echo "Available targets:"
	@echo "  make                       - Build all exercises"
	@echo "  make <exercise>            - Build one exercise (e.g., make 1-gpio-driver)"
	@echo "  make flash-<exercise>      - Flash via OpenOCD"
	@echo "  make debug-<exercise>      - Launch GDB session"
	@echo "  make disasm-<exercise>     - Disassemble the ELF"
	@echo "  make clean                 - Remove build artifacts"
	@echo ""
	@echo "Exercises found: $(EXERCISES)"

# --- Build a single exercise ---
# make 1-gpio-driver -> builds build/1-gpio-driver/firmware.bin
.SECONDEXPANSION:
$(EXERCISES): %: $(BUILD_DIR)/%/firmware.bin
	@$(SIZE) $(BUILD_DIR)/$*/firmware.elf

# --- Compile common C sources ---
$(BUILD_DIR)/common/%.o: $(COMMON_DIR)/%.c | $(BUILD_DIR)/common/src
	$(CC) $(CFLAGS) -c -o $@ $<

# --- Assemble startup ---
$(BUILD_DIR)/common/startup.o: $(COMMON_DIR)/startup.s | $(BUILD_DIR)/common
	$(AS) $(ASFLAGS) -o $@ $<

# --- Link exercise ---
# Uses second expansion ($$) to resolve wildcard at rule-execution time
$(BUILD_DIR)/%/firmware.elf: $(STARTUP_OBJ) $(COMMON_C_OBJ) $$(wildcard %/src/*.c) | $(BUILD_DIR)/%
	$(CC) $(CFLAGS) $(LDFLAGS) \
		-I$*/include \
		$(STARTUP_OBJ) $(COMMON_C_OBJ) $(wildcard $*/src/*.c) \
		-o $@

# --- Generate binary ---
$(BUILD_DIR)/%/firmware.bin: $(BUILD_DIR)/%/firmware.elf
	$(OBJCOPY) -O binary $< $@

# --- Create build directories ---
$(BUILD_DIR)/%:
	mkdir -p $@

$(BUILD_DIR)/common/src:
	mkdir -p $@

# ==============================================================================
# Flash and Debug
# ==============================================================================

# Flash a single exercise
flash-%: $(BUILD_DIR)/%/firmware.bin
	openocd -f $(OPENOCD_CFG) \
		-c "init" \
		-c "reset halt" \
		-c "load_image $< $(M4_LOAD_ADDR)" \
		-c "resume $(M4_LOAD_ADDR)" \
		-c "shutdown"

# Start OpenOCD + GDB for interactive debugging
debug-%: $(BUILD_DIR)/%/firmware.elf
	@echo "Starting OpenOCD in background..."
	@echo "In another terminal, run:"
	@echo "  $(GDB) $(BUILD_DIR)/$*/firmware.elf"
	@echo "  (gdb) target remote :3333"
	@echo "  (gdb) monitor reset halt"
	@echo "  (gdb) load"
	@echo "  (gdb) break main"
	@echo "  (gdb) continue"
	openocd -f $(OPENOCD_CFG)

# Disassemble an exercise's ELF
disasm-%: $(BUILD_DIR)/%/firmware.elf
	$(OBJDUMP) -d -S $< > $(BUILD_DIR)/$*/firmware.lst
	@echo "Disassembly written to $(BUILD_DIR)/$*/firmware.lst"

# ==============================================================================
# Clean
# ==============================================================================

clean:
	rm -rf $(BUILD_DIR)