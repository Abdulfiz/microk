# microk

![CI](https://github.com/Abdulfiz/microk/actions/workflows/ci.yml/badge.svg)

A small preemptive RTOS for ARM Cortex-M3, written in C. It runs and is
tested entirely in QEMU, so no hardware is needed.

## Features
- Boot code, vector table and linker script written from scratch
- Interrupt-driven UART output
- 1 ms system tick (SysTick)
- Preemptive round-robin scheduler with context switching in PendSV
- `task_delay()` and an idle task that sleeps the CPU (`wfi`)
- Counting semaphores and mutexes that block without wasting CPU time
- Automated tests that run in QEMU, plus GitHub Actions CI

## Build and run
```bash
sudo apt install gcc-arm-none-eabi qemu-system-arm make
make          # build the demo
make run      # run the demo in QEMU (quit with Ctrl-A then X)
make test     # run all automated tests
```

## Layout
| Folder | Contents |
|---|---|
| `arch/` | startup code, vector table, linker script |
| `drivers/` | UART driver |
| `src/` | kernel (scheduler, tasks, semaphores, mutexes) and demo `main.c` |
| `tests/` | one firmware image per test, run under QEMU |

## Known limitations / roadmap
- [ ] Task priorities (currently round-robin only)
- [ ] Priority inheritance for mutexes
- [ ] Message queues
- [ ] Software timers
- [ ] Stack-overflow detection
- [ ] Port to a real board (e.g. STM32 Nucleo)

## License
MIT
