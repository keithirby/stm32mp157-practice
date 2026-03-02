CC      = arm-none-eabi-gcc
AS      = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy

CFLAGS  = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
          -nostdlib -ffreestanding -Wall -Wextra -O2
LDFLAGS = -T common/linker.ld -nostdlib

# Auto-discover exercise directories (anything matching N-name pattern)
EXERCISES := $(sort $(wildcard [0-9]*-*))

# Common sources shared by all exercises
COMMON_C   := $(wildcard common/src/*.c)
COMMON_OBJ := $(COMMON_C:.c=.o)
STARTUP_OBJ := common/startup.o

# Default: build all exercises
all: $(EXERCISES)

# Each exercise builds to build/<name>/firmware.bin
.PHONY: $(EXERCISES)
$(EXERCISES): %: build/%/firmware.bin

build/%/firmware.bin: build/%/firmware.elf
	$(OBJCOPY) -O binary $< $@

build/%/firmware.elf: $(STARTUP_OBJ) $(COMMON_OBJ) $$(wildcard %/src/*.c) | build/%
	$(CC) $(CFLAGS) $(LDFLAGS) -Icommon/include -I$*/include \
		$(STARTUP_OBJ) $(COMMON_OBJ) $(wildcard $*/src/*.c) -o $@

build/%:
	mkdir -p $@

# Compile common objects
common/%.o: common/%.c
	$(CC) $(CFLAGS) -Icommon/include -c -o $@ $

common/startup.o: common/startup.s
	$(AS) -mcpu=cortex-m4 -mthumb -o $@ $

# Build + flash a single exercise
flash-%: build/%/firmware.bin
	openocd -f board/stm32mp15x_dk2.cfg \
		-c "init" -c "reset halt" \
		-c "load_image $< 0x10000000" \
		-c "resume 0x10000000" -c "shutdown"

clean:
	rm -rf build common/*.o common/src/*.o

.PHONY: all clean
.SECONDEXPANSION: