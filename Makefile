.PHONY: all clean run iso

CC = gcc
AS = nasm
LD = ld
RM = rm

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I./src/include -m64
ASFLAGS = -f bin
LDFLAGS = -T linker.ld -nostdlib -m elf_x86_64

BOOT_SRC = src/boot/boot.asm
KERNEL_SRC = src/kernel/main.c src/kernel/task_manager.c src/lib/stdio.c src/driver/driver.c src/gui/gui.c src/gui/xp_theme.c src/gui/xp_desktop.c src/apps/apps.c src/network/network.c src/filesystem/filesystem.c

BOOT_BIN = build/boot.bin
KERNEL_ELF = build/kernel.elf
KERNEL_OBJ = build/main.o build/task_manager.o build/stdio.o build/driver.o build/gui.o build/xp_theme.o build/xp_desktop.o build/apps.o build/network.o build/filesystem.o

ISO = build/ua-horizon.iso

all: $(BOOT_BIN) $(KERNEL_ELF) $(ISO)

$(BOOT_BIN): $(BOOT_SRC)
	@mkdir -p build
	$(AS) $(ASFLAGS) -o $@ $<

$(KERNEL_OBJ): $(KERNEL_SRC)
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $(filter %$(notdir $(basename $@)).c,$(KERNEL_SRC))

$(KERNEL_ELF): $(KERNEL_OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

$(ISO): $(BOOT_BIN) $(KERNEL_ELF)
	@mkdir -p build/isofiles/boot
	cp $< build/isofiles/boot/boot.bin
	cp $(KERNEL_ELF) build/isofiles/boot/kernel.elf
	genisoimage -quiet -boot-load-size 4 -no-emul-boot -boot-info-table -o $@ build/isofiles || true

run: $(ISO)
	qemu-system-i386 -cdrom $< -m 128M -display gtk &

clean:
	$(RM) -rf build/*

-include $(KERNEL_OBJ:.o=.d)