.PHONY: all clean run iso

CC = i686-elf-gcc
AS = nasm
LD = ld
RM = rm

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I./src/include
ASFLAGS = -f elf64
LDFLAGS = -T linker.ld -nostdlib --no-elf-strict-alignment

BOOT_SRC = src/boot/boot.asm
KERNEL_SRC = src/kernel/main.c src/kernel/task_manager.c src/lib/stdio.c src/driver/driver.c src/gui/gui.c src/gui/xp_theme.c src/gui/xp_desktop.c

KERNEL_OBJ = build/main.o build/task_manager.o build/stdio.o build/driver.o build/gui.o build/xp_theme.o build/xp_desktop.o

KERNEL_BIN = build/kernel.bin
BOOT_BIN = build/boot.bin
ISO = build/ua-horizon.iso

all: $(BOOT_BIN) $(KERNEL_BIN) $(ISO)

$(BOOT_OBJ): $(BOOT_SRC)
	@mkdir -p build
	$(AS) $(ASFLAGS) -o $@ $<

$(KERNEL_OBJ): $(KERNEL_SRC)
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $(filter %$(notdir $(basename $@)).c,$(KERNEL_SRC))

$(KERNEL_BIN): $(BOOT_OBJ) $(KERNEL_OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

$(ISO): $(BOOT_BIN) $(KERNEL_BIN)
	@mkdir -p build/isofiles/boot
	cp $< build/isofiles/boot/boot.bin
	cp kernel.elf build/isofiles/boot/
	genisoimage -quiet -boot-load-size 4 -no-emul-boot -boot-info-table -o $@ build/isofiles || \
	(isoinfo -x -R -o $< build/isofiles 2>/dev/null || true)

run: $(ISO)
	qemu-system-i386 -cdrom $< -m 128M -display gtk &

clean:
	$(RM) -rf build/*

-include $(KERNEL_OBJ:.o=.d)