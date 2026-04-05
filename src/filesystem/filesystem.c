#include "../include/filesystem.h"
#include "../include/driver.h"
#include "../include/string.h"
#include "../include/memory.h"

// Simple file system implementation
// Uses RAM for storage (in real OS, would use disk)

static fs_superblock_t superblock;
static fs_file_entry_t files[FS_MAX_FILES];
static uint8_t *data_area = NULL;
static int mounted = 0;

void fs_init(void) {
    // Initialize in-memory file system
    memory_set(&superblock, 0, sizeof(fs_superblock_t));
    memory_set(files, 0, sizeof(files));
    data_area = (uint8_t *)kmalloc(FS_DISK_SIZE);
    if (!data_area) {
        printf("FS: Failed to allocate memory\n");
        return;
    }
    mounted = 1;
    printf("FS: Initialized in-memory file system\n");
}

int fs_format(void) {
    if (!mounted) return -1;
    
    copy_str(superblock.signature, "UA-HORIZ");
    superblock.total_blocks = FS_DISK_SIZE / FS_BLOCK_SIZE;
    superblock.free_blocks = superblock.total_blocks - 1;  // Reserve superblock
    superblock.root_dir_block = 1;
    superblock.block_size = FS_BLOCK_SIZE;
    superblock.version = 1;
    
    memory_set(files, 0, sizeof(files));
    memory_set(data_area, 0, FS_DISK_SIZE);
    
    printf("FS: Formatted file system\n");
    return 0;
}

int fs_create_file(const char *name, uint32_t size) {
    if (!mounted || !name || name[0] == 0 || size == 0 || string_length(name) >= FS_MAX_NAME_LEN) return -1;

    // Check if already exists
    for (int j = 0; j < FS_MAX_FILES; j++) {
        if (strcmp(files[j].name, name) == 0) return -1;
    }

    // Find free slot
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (files[i].name[0] == 0) {
            copy_str(files[i].name, name);
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
            memory_set(&files[i], 0, sizeof(fs_file_entry_t));
            superblock.free_blocks++;
            printf("FS: Deleted file %s\n", name);
            return 0;
        }
    }
    return -1;  // Not found
}

int fs_write_file(const char *name, const void *data, uint32_t size) {
    if (!mounted || !data) return -1;
    
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (strcmp(files[i].name, name) == 0) {
            uint32_t to_write = size < files[i].size ? size : files[i].size;
            memory_copy(data_area + files[i].first_block * FS_BLOCK_SIZE, data, to_write);
            files[i].size = to_write;
            files[i].modified_at = 0;  // TODO: timestamp
            printf("FS: Wrote %d bytes to %s\n", to_write, name);
            return to_write;
        }
    }
    return -1;  // Not found
}

int fs_read_file(const char *name, void *buffer, uint32_t size) {
    if (!mounted || !buffer) return -1;
    
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (strcmp(files[i].name, name) == 0) {
            uint32_t to_read = size < files[i].size ? size : files[i].size;
            memory_copy(buffer, data_area + files[i].first_block * FS_BLOCK_SIZE, to_read);
            printf("FS: Read %u bytes from %s\n", to_read, name);
            return to_read;
        }
    }
    return -1;  // Not found
}

int fs_list_files(char *buffer, uint32_t buffer_size) {
    if (!mounted || !buffer) return -1;
    
    uint32_t offset = 0;
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (files[i].name[0] != 0) {
            // Simple formatting without snprintf
            copy_str(buffer + offset, files[i].name);
            offset += (uint32_t)string_length(files[i].name);
            if (offset < buffer_size - (uint32_t)20) {
                copy_str(buffer + offset, " (");
                offset += 2;
                char size_buf[12];
                uint_to_str(size_buf, files[i].size);
                copy_str(buffer + offset, size_buf);
                offset += string_length(size_buf);
                copy_str(buffer + offset, " bytes)");
                offset += 7;
                copy_str(buffer + offset, files[i].is_directory ? " [DIR]\n" : "\n");
                offset += files[i].is_directory ? 7 : 1;
            }
        }
    }
    return offset;
}

int fs_get_file_info(const char *name, fs_file_entry_t *info) {
    if (!mounted || !name || !info) return -1;
    
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (strcmp(files[i].name, name) == 0) {
            memory_copy(info, &files[i], sizeof(fs_file_entry_t));
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