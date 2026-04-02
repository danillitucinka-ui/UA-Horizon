#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "stdint.h"

#define FS_BLOCK_SIZE 512
#define FS_MAX_FILES 128
#define FS_MAX_NAME_LEN 32
#define FS_DISK_SIZE (50 * 1024 * 1024)  // 50MB

typedef struct {
    uint32_t size;
    uint32_t created_at;
    uint32_t modified_at;
    char name[FS_MAX_NAME_LEN];
    uint32_t first_block;
    int is_directory;
} fs_file_entry_t;

typedef struct {
    char signature[8];        // "UA-HORIZ"
    uint32_t total_blocks;
    uint32_t free_blocks;
    uint32_t root_dir_block;
    uint32_t block_size;
    uint32_t version;
} fs_superblock_t;

void fs_init(void);
int fs_format(void);
int fs_create_file(const char *name, uint32_t size);
int fs_delete_file(const char *name);
int fs_write_file(const char *name, const void *data, uint32_t size);
int fs_read_file(const char *name, void *buffer, uint32_t size);
int fs_list_files(char *buffer, uint32_t buffer_size);
int fs_get_file_info(const char *name, fs_file_entry_t *info);
int fs_mount(const char *device);

#endif