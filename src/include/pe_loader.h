#ifndef PE_LOADER_H
#define PE_LOADER_H

#include <stdint.h>
#include <stddef.h>

#define PE_MAGIC 0x4D5A
#define PE_SIGNATURE 0x50450000

typedef struct {
    uint16_t magic;
    uint16_t last_page_size;
    uint16_t page_count;
    uint16_t relocations;
    uint16_t header_size;
    uint16_t min_alloc;
    uint16_t max_alloc;
    uint16_t ss_init;
    uint16_t sp_init;
    uint16_t checksum;
    uint16_t ip_init;
    uint16_t cs_init;
    uint16_t reloc_table;
    uint16_t overlay;
    uint16_t reserved[4];
    uint16_t oem_id;
    uint16_t oem_info;
    uint16_t reserved2[10];
    uint32_t pe_header_offset;
} pe_dos_header_t;

typedef struct {
    uint32_t signature;
    uint16_t machine;
    uint16_t num_sections;
    uint32_t timestamp;
    uint32_t symbol_table;
    uint32_t num_symbols;
    uint16_t optional_header_size;
    uint16_t characteristics;
} pe_coff_header_t;

typedef struct {
    uint16_t magic;
    uint8_t major_linker;
    uint8_t minor_linker;
    uint32_t code_size;
    uint32_t init_data_size;
    uint32_t uninit_data_size;
    uint32_t entry_point;
    uint32_t code_base;
    uint32_t data_base;
    uint32_t image_base;
    uint32_t section_align;
    uint32_t file_align;
    uint16_t major_os;
    uint16_t minor_os;
    uint16_t major_image;
    uint16_t minor_image;
    uint16_t major_subsystem;
    uint16_t minor_subsystem;
    uint32_t win32_version;
    uint32_t image_size;
    uint32_t header_size;
    uint32_t checksum;
    uint16_t subsystem;
    uint16_t dll_chars;
    uint32_t stack_reserve;
    uint32_t stack_commit;
    uint32_t heap_reserve;
    uint32_t heap_commit;
    uint32_t loader_flags;
    uint32_t num_rva;
} pe_optional_header_t;

typedef struct {
    char name[8];
    uint32_t virtual_size;
    uint32_t virtual_addr;
    uint32_t raw_size;
    uint32_t raw_offset;
    uint32_t reloc_ptr;
    uint32_t line_ptr;
    uint16_t num_relocs;
    uint16_t num_lines;
    uint32_t characteristics;
} pe_section_t;

typedef struct {
    pe_dos_header_t dos_header;
    pe_coff_header_t coff_header;
    pe_optional_header_t optional_header;
    void *image_base;
    uint32_t image_size;
    uint32_t entry_point;
    int is_valid;
    char error_msg[128];
} pe_image_t;

pe_image_t *pe_load(const char *path);
pe_image_t *pe_load_from_memory(const void *data, size_t size);
int pe_execute(pe_image_t *image);
void pe_unload(pe_image_t *image);
int pe_is_valid(pe_image_t *image);
const char *pe_get_error(pe_image_t *image);

#endif
