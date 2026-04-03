# UA-Horizon OS

A modern operating system with graphical interface, inspired by Windows XP.

## Features

- **64-bit kernel** with custom bootloader
- **Memory management** with physical memory allocator
- **XP-style GUI** desktop with taskbar and start menu
- **File system** with 50MB capacity
- **Task manager** with priority scheduling
- **Network stack** with HTTP/HTTPS support
- **Shell** with internet commands
- **Applications**: Calculator, Text Editor, Browser, Games, File Manager, etc.
- **Internet support** with DNS, HTTP client, and EXE execution

## Quick Start

### Download Pre-built ISO

Download the latest ISO from [GitHub Releases](https://github.com/yourusername/ua-horizon/releases).

### Run in QEMU

```bash
qemu-system-i386 -cdrom ua-horizon-0.3.0.iso -m 128M
```

### Manual Build

#### Linux/macOS/Windows (WSL)
```bash
# Install dependencies
sudo apt-get install nasm gcc make genisoimage  # Linux
brew install nasm gcc make                      # macOS

# Build
chmod +x build_iso.sh
./build_iso.sh
```

#### GitHub Actions (Automatic)

The OS automatically builds on every push to `main` or `develop` branches.

#### Manual ISO Build

You can also trigger a manual build through GitHub Actions:

1. Go to the repository on GitHub
2. Click on the "Actions" tab
3. Click on "Build and Test" workflow
4. Click "Run workflow"
5. Optionally specify a custom version tag
6. Click "Run workflow"

#### Release Build

For releases, create a git tag:
```bash
git tag v0.3.0
git push origin v0.3.0
```

GitHub Actions will:
- Build the complete ISO
- Test it in QEMU
- Upload artifacts
- Create a GitHub release with the ISO

## Project Structure

```
src/
├── boot/          # Bootloader (ASM)
├── kernel/        # Core kernel (C)
├── gui/           # XP-style GUI
├── driver/        # Hardware drivers
├── lib/           # Standard libraries
├── network/       # TCP/IP stack & internet
├── apps/          # Built-in applications
└── include/       # Headers

.github/workflows/ # CI/CD pipeline
build_iso.sh       # Local build script
Makefile          # Build system
```

## Applications

- **Calculator** - Arithmetic operations
- **Text Editor** - File editing
- **Browser** - Text-based web browsing
- **Snake Game** - Classic game
- **File Manager** - File operations
- **System Monitor** - Resource usage
- **Terminal** - Command shell
- **Settings** - Configuration
- **Help** - Documentation

## Commands

```bash
UA-Horizon> help          # Show available commands
UA-Horizon> calc          # Launch calculator
UA-Horizon> browser       # Launch browser
UA-Horizon> wget <url>    # Download file
UA-Horizon> curl <url>    # Fetch webpage
UA-Horizon> ping <host>   # Network ping
UA-Horizon> exe <file>    # Execute script
UA-Horizon> tasks         # Show running tasks
```

## Development

### Prerequisites
- nasm (assembler)
- gcc (C compiler)
- make (build system)
- genisoimage (ISO creation)

### Build Process

1. **Bootloader**: `nasm` assembles `boot.asm`
2. **Kernel**: `gcc` compiles C sources to ELF
3. **ISO**: `genisoimage` creates bootable image
4. **Test**: QEMU validates the build

### Adding Applications

```c
// In src/apps/apps.c
int my_app_main(int argc, char *argv[]) {
    printf("My Application\n");
    return 0;
}

// Register in apps_init()
apps_register("myapp", my_app_main, "My cool application");
```

## Architecture

- **Real Mode** → **Protected Mode** → **Long Mode**
- **Multiboot2** compliant bootloader
- **64-bit** kernel with paging
- **Preemptive multitasking** scheduler
- **FAT-like** filesystem
- **TCP/IP** network stack

## Internet Features

- **HTTP/HTTPS client** for web access
- **DNS resolution** for hostnames
- **File downloads** with progress
- **Script execution** (EXE support)
- **Demo websites** (ua-horizon.org, google.com, github.com)

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make changes
4. Submit a pull request

## License

This project is open source. See LICENSE file for details.

## Screenshots

*(Add screenshots of the GUI and applications)*

## Roadmap

- [x] Basic kernel and bootloader
- [x] GUI desktop
- [x] File system
- [x] Task manager
- [x] Network stack
- [x] Applications suite
- [x] Internet support
- [ ] Sound drivers
- [ ] USB support
- [ ] Advanced networking
- [ ] Package manager

---

**UA-Horizon OS** - A complete operating system built from scratch! 🚀