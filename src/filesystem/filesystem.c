#include "../../include/filesystem.h"
#include "../../include/driver.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Simple file system implementation
// Uses RAM for storage (in real OS, would use disk)

static fs_superblock_t superblock;
static fs_file_entry_t files[FS_MAX_FILES];
static uint8_t *data_area = NULL;
static int mounted = 0;

void fs_init(void) {
    // Initialize in-memory file system
    memset(&superblock, 0, sizeof(fs_superblock_t));
    memset(files, 0, sizeof(files));
    data_area = (uint8_t *)malloc(FS_DISK_SIZE);
    if (!data_area) {
        printf("FS: Failed to allocate memory\n");
        return;
    }
    mounted = 1;
    printf("FS: Initialized in-memory file system\n");
}

int fs_format(void) {
    if (!mounted) return -1;
    
    strcpy(superblock.signature, "UA-HORIZ");
    superblock.total_blocks = FS_DISK_SIZE / FS_BLOCK_SIZE;
    superblock.free_blocks = superblock.total_blocks - 1;  // Reserve superblock
    superblock.root_dir_block = 1;
    superblock.block_size = FS_BLOCK_SIZE;
    superblock.version = 1;
    
    memset(files, 0, sizeof(files));
    memset(data_area, 0, FS_DISK_SIZE);
    
    printf("FS: Formatted file system\n");
    return 0;
}

int fs_create_file(const char *name, uint32_t size) {
    if (!mounted || strlen(name) >= FS_MAX_NAME_LEN) return -1;
    
    // Find free slot
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (files[i].name[0] == 0) {
            strcpy(files[i].name, name);
            files[i].size = size;
            files[i].is_directory = 0;
            files[i].first_block = 2 + i;  // Simple allocation
            files[i].created_at = 0;  // TODO: timestamp
            files[i].modified_at = 0;
            
            superblock.free_blocks--;
            printf("FS: Created file %s\n", name);
            return 0;
        }
    }
    return -1;  // No space
}

int fs_delete_file(const char *name) {
    if (!mounted) return -1;
    
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (strcmp(files[i].name, name) == 0) {
            memset(&files[i], 0, sizeof(fs_file_entry_t));
            superblock.free_blocks++;
            printf("FS: Deleted file %s\n", name);
            return 0;
        }
    }
    return -1;  // Not found
}

int fs_write_file(const char *name, const void *data, uint32_t size) {
    if (!mounted) return -1;
    
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (strcmp(files[i].name, name) == 0) {
            uint32_t to_write = size < files[i].size ? size : files[i].size;
            memcpy(data_area + files[i].first_block * FS_BLOCK_SIZE, data, to_write);
            files[i].size = to_write;
            files[i].modified_at = 0;  // TODO: timestamp
            printf("FS: Wrote %d bytes to %s\n", to_write, name);
            return to_write;
        }
    }
    return -1;  // Not found
}

int fs_read_file(const char *name, void *buffer, uint32_t size) {
    if (!mounted) return -1;
    
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (strcmp(files[i].name, name) == 0) {
            uint32_t to_read = size < files[i].size ? size : files[i].size;
            memcpy(buffer, data_area + files[i].first_block * FS_BLOCK_SIZE, to_read);
            printf("FS: Read %d bytes from %s\n", to_read, name);
            return to_read;
        }
    }
    return -1;  // Not found
}

int fs_list_files(char *buffer, uint32_t buffer_size) {
    if (!mounted || !buffer) return -1;
    
    int offset = 0;
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (files[i].name[0] != 0) {
            int len = snprintf(buffer + offset, buffer_size - offset, 
                             "%s (%d bytes)%s", 
                             files[i].name, files[i].size, 
                             files[i].is_directory ? " [DIR]\n" : "\n");
            if (len < 0 || offset + len >= buffer_size) break;
            offset += len;
        }
    }
    return offset;
}

int fs_get_file_info(const char *name, fs_file_entry_t *info) {
    if (!mounted || !info) return -1;
    
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (strcmp(files[i].name, name) == 0) {
            memcpy(info, &files[i], sizeof(fs_file_entry_t));
            return 0;
        }
    }
    return -1;  // Not found
}

int fs_mount(const char *device) {
    // Stub for mounting
    (void)device;
    mounted = 1;
    printf("FS: Mounted device %s\n", device);
    return 0;
}