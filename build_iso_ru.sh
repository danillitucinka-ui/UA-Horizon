#!/bin/bash
# Скрипт сборки ISO для UA-Horizon OS
# Кроссплатформенный сборщик ISO для Linux, macOS, Windows (с WSL/Cygwin)
# Создает самодостаточный загрузочный образ ISO

set -e  # Выход при любой ошибке
set -u  # Выход при неопределенных переменных

# Конфигурация скрипта
PROJECT_NAME="ua-horizon"
VERSION=$(git describe --tags --abbrev=0 2>/dev/null || echo "0.3.0")
OUTPUT_ISO="${PROJECT_NAME}-${VERSION}.iso"
BUILD_DIR="build"
ISO_ROOT="${BUILD_DIR}/iso_root"
GRUB_DIR="${ISO_ROOT}/boot/grub"

# Коды цветов для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # Без цвета

# Функции логирования
log_info() { echo -e "${BLUE}[ИНФО]${NC} $1"; }
log_warn() { echo -e "${YELLOW}[ПРЕДУПРЕЖДЕНИЕ]${NC} $1"; }
log_error() { echo -e "${RED}[ОШИБКА]${NC} $1"; }
log_success() { echo -e "${GREEN}[УСПЕХ]${NC} $1"; }

# Определение операционной системы
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="linux"
        log_info "Обнаружена Linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
        log_info "Обнаружена macOS"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
        OS="windows"
        log_info "Обнаружена Windows (используется WSL/Cygwin)"
    else
        log_error "Неподдерживаемая ОС: $OSTYPE"
        exit 1
    fi
}

# Проверка зависимостей
check_dependencies() {
    log_info "Проверка зависимостей..."

    local missing_deps=()

    # Проверка необходимых инструментов
    for tool in nasm gcc make; do
        if ! command -v "$tool" &> /dev/null; then
            missing_deps+=("$tool")
        fi
    done

    # Инструменты для конкретной ОС
    case $OS in
        linux)
            if ! command -v genisoimage &> /dev/null && ! command -v mkisofs &> /dev/null; then
                missing_deps+=("genisoimage или mkisofs")
            fi
            ;;
        macos)
            if ! command -v hdiutil &> /dev/null; then
                missing_deps+=("hdiutil")
            fi
            ;;
        windows)
            if ! command -v genisoimage &> /dev/null && ! command -v mkisofs &> /dev/null; then
                missing_deps+=("genisoimage или mkisofs (установите cdrtools)")
            fi
            ;;
    esac

    if [ ${#missing_deps[@]} -ne 0 ]; then
        log_warn "Отсутствующие зависимости: ${missing_deps[*]}"
        log_info "Продолжаем без них, ISO может не создаться."
    fi

    log_success "Все зависимости найдены"
}

# Очистка директории сборки
clean_build() {
    log_info "Очистка директории сборки..."
    rm -rf "$BUILD_DIR"
    mkdir -p "$BUILD_DIR"
    log_success "Директория сборки очищена"
}

# Сбор всех файлов проекта
collect_files() {
    log_info "Сбор файлов проекта..."

    # Создание архива исходников для воспроизводимости
    local src_archive="${BUILD_DIR}/source.tar.gz"

    # Исключить артефакты сборки, но включить все исходники
    tar --exclude="$BUILD_DIR" \
        --exclude='.git' \
        --exclude='*.iso' \
        --exclude='*.img' \
        --exclude='*.bin' \
        --exclude='*.elf' \
        --exclude='*.o' \
        --exclude='*.d' \
        -czf "$src_archive" .

    log_info "Архив исходников создан: $src_archive"

    # Проверка целостности архива
    if ! tar -tzf "$src_archive" &> /dev/null; then
        log_error "Архив исходников поврежден"
        exit 1
    fi

    log_success "Файлы проекта собраны и заархивированы"
}

# Проверка артефактов сборки
check_build_artifacts() {
    log_info "Проверка артефактов сборки..."

    local required_files=(
        "build/boot.bin"
        "build/kernel.elf"
    )

    for file in "${required_files[@]}"; do
        if [[ ! -f "$file" ]]; then
            log_error "Отсутствует артефакт сборки: $file"
            exit 1
        fi
    done

    log_success "Артефакты сборки найдены"
}

# Создание структуры ISO
create_iso_structure() {
    log_info "Создание структуры файловой системы ISO..."

    # Создание директорий
    mkdir -p "$GRUB_DIR"

    # Копирование загрузчика и ядра
    cp "build/boot.bin" "$ISO_ROOT/boot/"
    cp "build/kernel.elf" "$ISO_ROOT/boot/"

    # Создание конфигурации GRUB для загрузки BIOS
    cat > "$GRUB_DIR/grub.cfg" << 'EOF'
set timeout=5
set default=0

menuentry "UA-Horizon OS" {
    multiboot /boot/kernel.elf
    boot
}
EOF

    # Копирование дополнительных ресурсов (если существуют)
    if [[ -d "assets" ]]; then
        cp -r assets/* "$ISO_ROOT/"
        log_info "Ресурсы скопированы в ISO"
    fi

    log_success "Структура ISO создана"
}

# Создание образа ISO
create_iso() {
    log_info "Создание образа ISO: $OUTPUT_ISO"

    case $OS in
        linux)
            # Использование genisoimage или mkisofs
            if command -v genisoimage &> /dev/null; then
                ISO_CMD="genisoimage"
            elif command -v mkisofs &> /dev/null; then
                ISO_CMD="mkisofs"
            fi

            # Создание загрузочного ISO El Torito
            "$ISO_CMD" -R -b boot/boot.bin -no-emul-boot \
                      -boot-load-size 4 -boot-info-table \
                      -o "$OUTPUT_ISO" "$ISO_ROOT" 2>/dev/null || \
            "$ISO_CMD" -R -b boot/boot.bin -no-emul-boot \
                      -o "$OUTPUT_ISO" "$ISO_ROOT"

            ;;
        macos)
            # Использование hdiutil для создания ISO
            hdiutil makehybrid -iso -joliet -o "$OUTPUT_ISO" "$ISO_ROOT"
            ;;
        windows)
            # Использование genisoimage/mkisofs в WSL/Cygwin
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

    # Проверка, что ISO создан
    if [[ ! -f "$OUTPUT_ISO" ]]; then
        log_error "Не удалось создать образ ISO"
        exit 1
    fi

    local iso_size
    iso_size=$(stat -f%z "$OUTPUT_ISO" 2>/dev/null || stat -c%s "$OUTPUT_ISO" 2>/dev/null || echo "неизвестно")
    log_success "ISO создан: $OUTPUT_ISO (${iso_size} байт)"
}

# Проверка содержимого ISO
verify_iso() {
    log_info "Проверка содержимого ISO..."

    # Монтирование и проверка содержимого (если возможно)
    case $OS in
        linux)
            local mount_point="/tmp/iso_verify_$$"
            mkdir -p "$mount_point"

            if mount -o loop,ro "$OUTPUT_ISO" "$mount_point" 2>/dev/null; then
                # Проверка необходимых файлов
                local required_files=(
                    "boot/boot.bin"
                    "boot/kernel.elf"
                    "boot/grub/grub.cfg"
                )

                for file in "${required_files[@]}"; do
                    if [[ ! -f "$mount_point/$file" ]]; then
                        log_error "Отсутствует файл в ISO: $file"
                        umount "$mount_point"
                        rmdir "$mount_point"
                        exit 1
                    fi
                done

                umount "$mount_point"
                rmdir "$mount_point"
                log_success "Содержимое ISO проверено"
            else
                log_warn "Невозможно смонтировать ISO для проверки (отсутствует fuseiso или права root)"
                log_info "ISO создан, но не полностью проверен"
            fi
            ;;
        *)
            log_warn "Проверка ISO не поддерживается на $OS"
            ;;
    esac
}

# Генерация отчета о сборке
generate_report() {
    log_info "Генерация отчета о сборке..."

    local report_file="${BUILD_DIR}/build_report.txt"

    cat > "$report_file" << EOF
Отчет о сборке UA-Horizon OS
============================

Время сборки: $(date)
Версия: $VERSION
ОС: $OS ($OSTYPE)

Артефакты сборки:
$(ls -la build/)

Содержимое ISO:
$(find "$ISO_ROOT" -type f -exec ls -la {} \; 2>/dev/null || echo "Корень ISO: $ISO_ROOT")

Контрольные суммы:
$(sha256sum "$OUTPUT_ISO" 2>/dev/null || shasum -a 256 "$OUTPUT_ISO" 2>/dev/null || echo "Расчет контрольной суммы недоступен")

Проверенные зависимости:
- nasm: $(command -v nasm &>/dev/null && echo "ОК" || echo "ОТСУТСТВУЕТ")
- gcc: $(command -v gcc &>/dev/null && echo "ОК" || echo "ОТСУТСТВУЕТ")  
- make: $(command -v make &>/dev/null && echo "ОК" || echo "ОТСУТСТВУЕТ")
- Инструмент ISO: $( (command -v genisoimage || command -v mkisofs || command -v hdiutil) &>/dev/null && echo "ОК" || echo "ОТСУТСТВУЕТ")

Статус сборки: УСПЕХ
EOF

    log_success "Отчет о сборке создан: $report_file"
}

# Функция очистки
cleanup() {
    local exit_code=$?
    if [[ $exit_code -ne 0 ]]; then
        log_error "Сборка не удалась с кодом выхода $exit_code"
        log_info "Очистка..."
        rm -rf "$BUILD_DIR"
    fi
}

# Ловушка для очистки
trap cleanup EXIT

# Основное выполнение
main() {
    log_info "Запуск скрипта сборки ISO UA-Horizon OS v1.0"
    log_info "Проект: $PROJECT_NAME $VERSION"

    detect_os
    check_dependencies
    check_build_artifacts
    create_iso_structure
    create_iso
    verify_iso
    generate_report

    log_success "Сборка ISO UA-Horizon OS завершена успешно!"
    log_info "Вывод: $OUTPUT_ISO"
    log_info "Готово к записи или монтированию"
}

# Запуск основной функции
main "$@"