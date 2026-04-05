#!/bin/bash
# Build ISO script for UA-Horizon OS

# Create ISO directory structure
mkdir -p iso/boot/grub

# Copy kernel and boot files
cp build/boot.bin iso/boot/
cp build/kernel.elf iso/boot/

# Create GRUB config
cat > iso/boot/grub/grub.cfg << EOF
set timeout=0
set default=0

menuentry "UA-Horizon" {
    multiboot /boot/kernel.elf
    module /boot/boot.bin
    boot
}
EOF

# Build ISO using xorriso or mkisofs
if command -v xorriso &> /dev/null; then
    xorriso -as mkisofs -b boot/grub/i386-pc/eltorito.img \
            -no-emul-boot -boot-load-size 4 -boot-info-table \
            -o build/ua-horizon.iso iso/
elif command -v mkisofs &> /dev/null; then
    mkisofs -o build/ua-horizon.iso -b boot/grub/i386-pc/eltorito.img \
            -no-emul-boot -boot-load-size 4 -boot-info-table iso/
else
    echo "Error: Neither xorriso nor mkisofs found. Install xorriso."
    exit 1
fi

echo "ISO created: build/ua-horizon.iso"