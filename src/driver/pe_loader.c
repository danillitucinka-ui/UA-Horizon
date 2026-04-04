#include "../include/pe_loader.h"
#include "../include/memory.h"
#include "../include/stdio.h"
#include "../include/filesystem.h"

pe_image_t *pe_load(const char *path) {
    if (!path) return NULL;

    file_t *file = fs_open(path, FS_READ);
    if (!file) {
        return NULL;
    }

    size_t file_size = fs_get_size(file);
    if (file_size < sizeof(pe_dos_header_t)) {
        fs_close(file);
        return NULL;
    }

    void *buffer = kmalloc(file_size);
    if (!buffer) {
        fs_close(file);
        return NULL;
    }

    if (fs_read(file, buffer, file_size) != file_size) {
        kfree(buffer);
        fs_close(file);
        return NULL;
    }
    fs_close(file);

    pe_image_t *image = pe_load_from_memory(buffer, file_size);
    kfree(buffer);
    return image;
}

pe_image_t *pe_load_from_memory(const void *data, size_t size) {
    if (!data || size < sizeof(pe_dos_header_t)) {
        return NULL;
    }

    pe_image_t *image = (pe_image_t *)kmalloc(sizeof(pe_image_t));
    if (!image) {
        return NULL;
    }

    kmemset(image, 0, sizeof(pe_image_t));

    const pe_dos_header_t *dos = (const pe_dos_header_t *)data;
    if (dos->magic != PE_MAGIC) {
        image->is_valid = 0;
        kstrcpy(image->error_msg, "Invalid DOS magic");
        return image;
    }

    if (dos->pe_header_offset + sizeof(pe_coff_header_t) > size) {
        image->is_valid = 0;
        kstrcpy(image->error_msg, "PE header out of bounds");
        return image;
    }

    const pe_coff_header_t *coff = (const pe_coff_header_t *)((const uint8_t *)data + dos->pe_header_offset);
    if (coff->signature != PE_SIGNATURE) {
        image->is_valid = 0;
        kstrcpy(image->error_msg, "Invalid PE signature");
        return image;
    }

    if (coff->optional_header_size < sizeof(pe_optional_header_t)) {
        image->is_valid = 0;
        kstrcpy(image->error_msg, "Optional header too small");
        return image;
    }

    kmemcpy(&image->dos_header, dos, sizeof(pe_dos_header_t));
    kmemcpy(&image->coff_header, coff, sizeof(pe_coff_header_t));
    kmemcpy(&image->optional_header, 
            (const uint8_t *)data + dos->pe_header_offset + sizeof(pe_coff_header_t),
            sizeof(pe_optional_header_t));

    image->image_size = image->optional_header.image_size;
    image->entry_point = image->optional_header.entry_point;

    void *image_base = kmalloc(image->image_size);
    if (!image_base) {
        image->is_valid = 0;
        kstrcpy(image->error_msg, "Failed to allocate image memory");
        return image;
    }
    kmemset(image_base, 0, image->image_size);

    image->image_base = image_base;

    const pe_section_t *sections = (const pe_section_t *)((const uint8_t *)data + 
        dos->pe_header_offset + sizeof(pe_coff_header_t) + coff->optional_header_size);

    for (uint16_t i = 0; i < coff->num_sections; i++) {
        const pe_section_t *sec = &sections[i];
        if (sec->raw_size > 0 && sec->raw_offset + sec->raw_size <= size) {
            uint8_t *dest = (uint8_t *)image_base + sec->virtual_addr;
            const uint8_t *src = (const uint8_t *)data + sec->raw_offset;
            kmemcpy(dest, src, sec->raw_size);
        }
    }

    image->is_valid = 1;
    return image;
}

int pe_execute(pe_image_t *image) {
    if (!image || !image->is_valid) {
        return -1;
    }

    printf("Executing PE image at %p, entry: 0x%08X\n", 
           image->image_base, image->entry_point);

    uint8_t *entry = (uint8_t *)image->image_base + image->entry_point;

    __asm__ volatile (
        "call *%0\n"
        :
        : "r"(entry)
        : "memory"
    );

    return 0;
}

void pe_unload(pe_image_t *image) {
    if (image) {
        if (image->image_base) {
            kfree(image->image_base);
        }
        kfree(image);
    }
}

int pe_is_valid(pe_image_t *image) {
    return image && image->is_valid;
}

const char *pe_get_error(pe_image_t *image) {
    if (image) {
        return image->error_msg;
    }
    return "No image";
}
