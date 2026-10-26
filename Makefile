CC      = arm-none-eabi-gcc
CFLAGS  = -mcpu=cortex-m3 -mthumb -O1 -g -Wall -Wextra -ffreestanding \
          -ffunction-sections -fdata-sections -Isrc -Idrivers
LDFLAGS = -T arch/linker.ld -nostartfiles -nostdlib -Wl,--gc-sections
QEMU    = qemu-system-arm -machine lm3s6965evb -cpu cortex-m3 -nographic \
          -semihosting-config enable=on,target=native

SRCS = arch/startup.c drivers/uart.c

all: build/microk.elf

build/microk.elf: $(SRCS) src/main.c arch/linker.ld
	@mkdir -p build
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRCS) src/main.c -lgcc -o $@

run: build/microk.elf
	@echo "(press Ctrl-A then X to quit QEMU)"
	$(QEMU) -kernel build/microk.elf

clean:
	rm -rf build

.PHONY: all run clean
