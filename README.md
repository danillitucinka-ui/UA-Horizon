# UA-Horizon OS

Ukrainian National Operating System with graphical user interface.

## Build

```bash
make all
```

## Run (QEMU)

```bash
make run
```

## Features

- 64-bit Long Mode kernel
- Multiboot2 compliant bootloader
- Basic GUI subsystem
- Driver framework
- ACPI support
- VESA graphics

## Project Structure

```
src/
├── boot/     - Bootloader (ASM)
├── kernel/   - Kernel core (C)
├── gui/      - GUI subsystem
├── driver/   - Device drivers
├── lib/      - Standard library
└── include/  - Headers
```

## Roadmap

See ROADMAP.md for development plan.