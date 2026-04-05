.PHONY: all clean run iso

CC = gcc
AS = nasm
LD = ld
RM = rm

CFLAGS = -std=gnu99 -I./src/include -m64 -fno-stack-protector -fno-ident
ASFLAGS = -f bin
LDFLAGS = -T linker.ld -nostdlib -m elf_x86_64 -s

BOOT_SRC = src/boot/boot.asm
KERNEL_SRC = src/kernel/main.c src/kernel/task_manager.c src/kernel/shell.c src/lib/stdio.c src/lib/memory.c src/lib/string.c src/driver/driver.c src/driver/pe_loader.c src/gui/gui.c src/gui/xp_theme.c src/gui/xp_desktop.c src/apps/apps.c src/network/network.c src/filesystem/filesystem.c

BOOT_BIN = build/boot.bin
KERNEL_ELF = build/kernel.elf
KERNEL_OBJ = build/main.o build/task_manager.o build/shell.o build/stdio.o build/memory.o build/string.o build/driver.o build/pe_loader.o build/gui.o build/xp_theme.o build/xp_desktop.o build/apps.o build/network.o build/filesystem.o

ISO = build/ua-horizon.iso

all: $(BOOT_BIN) $(KERNEL_ELF) $(ISO)

$(BOOT_BIN): $(BOOT_SRC)
	@mkdir -p build
	$(AS) $(ASFLAGS) -o $@ $<

build/main.o: src/kernel/main.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

build/task_manager.o: src/kernel/task_manager.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

build/shell.o: src/kernel/shell.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

build/stdio.o: src/lib/stdio.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

build/memory.o: src/lib/memory.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

build/string.o: src/lib/string.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

build/driver.o: src/driver/driver.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

build/pe_loader.o: src/driver/pe_loader.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

build/gui.o: src/gui/gui.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

build/xp_theme.o: src/gui/xp_theme.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

build/xp_desktop.o: src/gui/xp_desktop.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

build/apps.o: src/apps/apps.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

build/network.o: src/network/network.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

build/filesystem.o: src/filesystem/filesystem.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

$(KERNEL_ELF): $(KERNEL_OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

$(ISO): $(BOOT_BIN) $(KERNEL_ELF)
	chmod +x build_iso_ru.sh
	./build_iso_ru.sh || true

run: $(ISO)
	qemu-system-x86_64 -cdrom $< -m 128M -display gtk &

clean:
	$(RM) -rf build/*

-include $(KERNEL_OBJ:.o=.d)