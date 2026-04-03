# UA-Horizon OS Roadmap

## Version 0.1.x - Foundation (Current)
- [x] Bootloader (16-bit Real Mode → 32-bit Protected → 64-bit Long Mode)
- [x] Multiboot2 compliance
- [x] Basic kernel skeleton
- [x] Terminal output (VGA text mode)
- [x] Basic I/O (inb/outb)

## Version 0.2.x - Memory & Drivers
- [x] Physical memory manager (PMM) - Basic bitmap allocator
- [ ] Virtual memory manager (VMM) - Basic identity mapping
- [x] PIT (Programmable Interval Timer) - Basic timer setup
- [x] Keyboard driver (PS/2) - Polling-based input
- [ ] VESA graphics mode support

## Version 0.3.x - GUI Core
- [ ] Window manager
- [ ] Basic UI components (buttons, labels)
- [ ] Event system (mouse/keyboard)
- [ ] Taskbar
- [ ] Basic fonts

## Version 0.4.x - System Calls
- [ ] System call interface
- [ ] Basic filesystem (FAT12/16)
- [ ] Shell/CLI

## Version 0.5.x - Polish
- [ ] ACPI support
- [ ] PCI enumeration
- [ ] Sound driver (optional)
- [ ] Network stack (optional)

## Contributing
1. Fork the repo
2. Create feature branch
3. Make changes
4. Submit PR

## Building
```bash
make all       # Build everything
make run       # Run in QEMU
make clean     # Clean build artifacts
```

## CI/CD
GitHub Actions automatically:
- Builds bootloader and kernel
- Runs linting
- Validates assembler and C syntax

## Contact
- Issues: GitHub Issues
- Email: See profile