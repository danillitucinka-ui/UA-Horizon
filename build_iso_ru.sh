#!/bin/bash
# Build ISO script for UA-Horizon OS

# Create ISO directory structure
mkdir -p iso/boot/grub

# Copy kernel and boot files
cp build/kernel.elf iso/boot/

# Create GRUB config
cat > iso/boot/grub/grub.cfg << EOF
set timeout=0
set default=0

menuentry "UA-Horizon" {
    multiboot /boot/kernel.elf
    boot
}
EOF

# Build ISO using grub-mkrescue if available, else xorriso
if command -v grub-mkrescue &> /dev/null; then
    grub-mkrescue -o build/ua-horizon.iso iso/
elif command -v xorriso &> /dev/null; then
    # Install GRUB for xorriso
    grub-install --target=i386-pc --boot-directory=iso/boot --install-modules="multiboot" --locales="" --themes="" iso/
    xorriso -as mkisofs -b boot/grub/i386-pc/eltorito.img \
            -no-emul-boot -boot-load-size 4 -boot-info-table \
            -o build/ua-horizon.iso iso/
else
    echo "Error: Neither grub-mkrescue nor xorriso found."
    exit 1
fi

echo "ISO created: build/ua-horizon.iso"