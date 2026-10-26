CC      = arm-none-eabi-gcc
CFLAGS  = -mcpu=cortex-m3 -mthumb -O1 -g -Wall -Wextra -ffreestanding \
          -ffunction-sections -fdata-sections -Isrc -Idrivers
LDFLAGS = -T arch/linker.ld -nostartfiles -nostdlib -Wl,--gc-sections
QEMU    = qemu-system-arm -machine lm3s6965evb -cpu cortex-m3 -nographic \
          -semihosting-config enable=on,target=native

SRCS  = arch/startup.c drivers/uart.c src/kernel.c
TESTS = $(basename $(notdir $(wildcard tests/test_*.c)))

all: build/microk.elf

build/microk.elf: $(SRCS) src/main.c arch/linker.ld
	@mkdir -p build
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRCS) src/main.c -lgcc -o $@

# each tests/test_xxx.c becomes its own firmware image build/test_xxx.elf
build/test_%.elf: tests/test_%.c $(SRCS) src/*.h tests/test.h arch/linker.ld
	@mkdir -p build
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRCS) $< -lgcc -o $@

run: build/microk.elf
	@echo "(press Ctrl-A then X to quit QEMU)"
	$(QEMU) -kernel build/microk.elf

# run every test in QEMU; a test passes if QEMU exits with status 0
test: $(addprefix build/,$(addsuffix .elf,$(TESTS)))
	@fail=0; for t in $(TESTS); do \
	  echo "== $$t"; \
	  out=$$(timeout 20 $(QEMU) -kernel build/$$t.elf </dev/null 2>/dev/null); rc=$$?; \
	  echo "$$out" | tr -d '\r'; \
	  if [ $$rc -ne 0 ]; then echo "!! $$t FAILED (exit code $$rc)"; fail=1; fi; \
	done; \
	if [ $$fail -eq 0 ]; then echo "ALL SUITES PASSED"; else exit 1; fi

clean:
	rm -rf build

.PHONY: all run test clean
