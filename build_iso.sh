#!/bin/bash
# UA-Horizon OS Build Script
# Cross-platform ISO builder for Linux, macOS, Windows (with WSL/Cygwin)
# Produces a self-contained bootable ISO image

set -e  # Exit on any error
set -u  # Exit on undefined variables

# Script configuration
PROJECT_NAME="ua-horizon"
VERSION=$(git describe --tags --abbrev=0 2>/dev/null || echo "0.3.0")
OUTPUT_ISO="${PROJECT_NAME}-${VERSION}.iso"
BUILD_DIR="build"
ISO_ROOT="${BUILD_DIR}/iso_root"
GRUB_DIR="${ISO_ROOT}/boot/grub"

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
log_warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }
log_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }

# Detect operating system
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="linux"
        log_info "Detected Linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
        log_info "Detected macOS"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
        OS="windows"
        log_info "Detected Windows (using WSL/Cygwin)"
    else
        log_error "Unsupported OS: $OSTYPE"
        exit 1
    fi
}

# Check dependencies
check_dependencies() {
    log_info "Checking dependencies..."

    local missing_deps=()

    # Check for required tools
    for tool in nasm gcc make; do
        if ! command -v "$tool" &> /dev/null; then
            missing_deps+=("$tool")
        fi
    done

    # OS-specific tools
    case $OS in
        linux)
            if ! command -v genisoimage &> /dev/null && ! command -v mkisofs &> /dev/null; then
                missing_deps+=("genisoimage or mkisofs")
            fi
            ;;
        macos)
            if ! command -v hdiutil &> /dev/null; then
                missing_deps+=("hdiutil")
            fi
            ;;
        windows)
            if ! command -v genisoimage &> /dev/null && ! command -v mkisofs &> /dev/null; then
                missing_deps+=("genisoimage or mkisofs (install cdrtools)")
            fi
            ;;
    esac

    if [ ${#missing_deps[@]} -ne 0 ]; then
        log_error "Missing dependencies: ${missing_deps[*]}"
        log_info "Please install required tools:"
        case $OS in
            linux)
                echo "  Ubuntu/Debian: sudo apt-get install nasm gcc make genisoimage"
                echo "  Fedora/CentOS: sudo dnf install nasm gcc make genisoimage"
                ;;
            macos)
                echo "  macOS: Install Command Line Tools: xcode-select --install"
                echo "  Or use Homebrew: brew install nasm gcc make"
                ;;
            windows)
                echo "  Windows: Install WSL or Cygwin, then apt-get install nasm gcc make genisoimage"
                ;;
        esac
        exit 1
    fi

    log_success "All dependencies found"
}

# Clean build directory
clean_build() {
    log_info "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    mkdir -p "$BUILD_DIR"
    log_success "Build directory cleaned"
}

# Collect all project files
collect_files() {
    log_info "Collecting project files..."

    # Create source tarball for reproducibility
    local src_archive="${BUILD_DIR}/source.tar.gz"

    # Exclude build artifacts, but include all source
    tar --exclude="$BUILD_DIR" \
        --exclude='.git' \
        --exclude='*.iso' \
        --exclude='*.img' \
        --exclude='*.bin' \
        --exclude='*.elf' \
        --exclude='*.o' \
        --exclude='*.d' \
        -czf "$src_archive" .

    log_info "Source archive created: $src_archive"

    # Verify archive integrity
    if ! tar -tzf "$src_archive" &> /dev/null; then
        log_error "Source archive is corrupted"
        exit 1
    fi

    log_success "Project files collected and archived"
}

# Build the kernel and bootloader
build_project() {
    log_info "Building UA-Horizon OS..."

    # Build using Makefile
    if ! make all; then
        log_error "Build failed"
        exit 1
    fi

    # Verify build artifacts
    local required_files=(
        "build/boot.bin"
        "build/kernel.elf"
    )

    for file in "${required_files[@]}"; do
        if [[ ! -f "$file" ]]; then
            log_error "Missing build artifact: $file"
            exit 1
        fi
    done

    log_success "Build completed successfully"
}

# Create ISO structure
create_iso_structure() {
    log_info "Creating ISO filesystem structure..."

    # Create directories
    mkdir -p "$GRUB_DIR"

    # Copy bootloader and kernel
    cp "build/boot.bin" "$ISO_ROOT/boot/"
    cp "build/kernel.elf" "$ISO_ROOT/boot/"

    # Create GRUB configuration for BIOS boot
    cat > "$GRUB_DIR/grub.cfg" << 'EOF'
set timeout=5
set default=0

menuentry "UA-Horizon OS" {
    multiboot /boot/kernel.elf
    boot
}
EOF

    # Copy any additional assets (if exist)
    if [[ -d "assets" ]]; then
        cp -r assets/* "$ISO_ROOT/"
        log_info "Assets copied to ISO"
    fi

    log_success "ISO structure created"
}

# Create the ISO image
create_iso() {
    log_info "Creating ISO image: $OUTPUT_ISO"

    case $OS in
        linux)
            # Use genisoimage or mkisofs
            if command -v genisoimage &> /dev/null; then
                ISO_CMD="genisoimage"
            elif command -v mkisofs &> /dev/null; then
                ISO_CMD="mkisofs"
            fi

            # Create El Torito bootable ISO
            "$ISO_CMD" -R -b boot/boot.bin -no-emul-boot \
                      -boot-load-size 4 -boot-info-table \
                      -o "$OUTPUT_ISO" "$ISO_ROOT" 2>/dev/null || \
            "$ISO_CMD" -R -b boot/boot.bin -no-emul-boot \
                      -o "$OUTPUT_ISO" "$ISO_ROOT"

            ;;
        macos)
            # Use hdiutil to create ISO
            hdiutil makehybrid -iso -joliet -o "$OUTPUT_ISO" "$ISO_ROOT"
            ;;
        windows)
            # Use genisoimage/mkisofs in WSL/Cygwin
            if command -v genisoimage &> /dev/null; then
                genisoimage -R -b boot/boot.bin -no-emul-boot \
                           -boot-load-size 4 -boot-info-table \
                           -o "$OUTPUT_ISO" "$ISO_ROOT"
            elif command -v mkisofs &> /dev/null; then
                mkisofs -R -b boot/boot.bin -no-emul-boot \
                       -boot-load-size 4 -boot-info-table \
                       -o "$OUTPUT_ISO" "$ISO_ROOT"
            fi
            ;;
    esac

    # Verify ISO was created
    if [[ ! -f "$OUTPUT_ISO" ]]; then
        log_error "Failed to create ISO image"
        exit 1
    fi

    local iso_size
    iso_size=$(stat -f%z "$OUTPUT_ISO" 2>/dev/null || stat -c%s "$OUTPUT_ISO" 2>/dev/null || echo "unknown")
    log_success "ISO created: $OUTPUT_ISO (${iso_size} bytes)"
}

# Verify ISO contents
verify_iso() {
    log_info "Verifying ISO contents..."

    # Mount and check contents (if possible)
    case $OS in
        linux)
            local mount_point="/tmp/iso_verify_$$"
            mkdir -p "$mount_point"

            if mount -o loop,ro "$OUTPUT_ISO" "$mount_point" 2>/dev/null; then
                # Check for required files
                local required_files=(
                    "boot/boot.bin"
                    "boot/kernel.elf"
                    "boot/grub/grub.cfg"
                )

                for file in "${required_files[@]}"; do
                    if [[ ! -f "$mount_point/$file" ]]; then
                        log_error "Missing file in ISO: $file"
                        umount "$mount_point"
                        rmdir "$mount_point"
                        exit 1
                    fi
                done

                umount "$mount_point"
                rmdir "$mount_point"
                log_success "ISO contents verified"
            else
                log_warn "Cannot mount ISO for verification (missing fuseiso or root permissions)"
                log_info "ISO created but not fully verified"
            fi
            ;;
        *)
            log_warn "ISO verification not supported on $OS"
            ;;
    esac
}

# Generate build report
generate_report() {
    log_info "Generating build report..."

    local report_file="${BUILD_DIR}/build_report.txt"

    cat > "$report_file" << EOF
UA-Horizon OS Build Report
==========================

Build Time: $(date)
Version: $VERSION
OS: $OS ($OSTYPE)

Build Artifacts:
$(ls -la build/)

ISO Contents:
$(find "$ISO_ROOT" -type f -exec ls -la {} \; 2>/dev/null || echo "ISO root: $ISO_ROOT")

Checksums:
$(sha256sum "$OUTPUT_ISO" 2>/dev/null || shasum -a 256 "$OUTPUT_ISO" 2>/dev/null || echo "Checksum calculation not available")

Dependencies Verified:
- nasm: $(command -v nasm &>/dev/null && echo "OK" || echo "MISSING")
- gcc: $(command -v gcc &>/dev/null && echo "OK" || echo "MISSING")  
- make: $(command -v make &>/dev/null && echo "OK" || echo "MISSING")
- ISO tool: $( (command -v genisoimage || command -v mkisofs || command -v hdiutil) &>/dev/null && echo "OK" || echo "MISSING")

Build Status: SUCCESS
EOF

    log_success "Build report generated: $report_file"
}

# Cleanup function
cleanup() {
    local exit_code=$?
    if [[ $exit_code -ne 0 ]]; then
        log_error "Build failed with exit code $exit_code"
        log_info "Cleaning up..."
        rm -rf "$BUILD_DIR"
    fi
}

# Trap for cleanup
trap cleanup EXIT

# Main execution
main() {
    log_info "Starting UA-Horizon OS ISO build script v1.0"
    log_info "Project: $PROJECT_NAME $VERSION"

    detect_os
    check_dependencies
    clean_build
    collect_files
    build_project
    create_iso_structure
    create_iso
    verify_iso
    generate_report

    log_success "UA-Horizon OS ISO build completed successfully!"
    log_info "Output: $OUTPUT_ISO"
    log_info "Ready for burning or mounting"
}

# Run main function
main "$@"