#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>

// Memory constants
#define PAGE_SIZE 4096
#define TOTAL_MEMORY_MB 64
#define TOTAL_MEMORY_BYTES (TOTAL_MEMORY_MB * 1024 * 1024)
#define TOTAL_PAGES (TOTAL_MEMORY_BYTES / PAGE_SIZE)

// Bitmap for page allocation (1 bit per page)
#define BITMAP_SIZE (TOTAL_PAGES / 8)

// Memory regions
#define KERNEL_START 0x100000  // 1MB
#define KERNEL_END 0x200000    // 2MB
#define HEAP_START 0x200000    // 2MB
#define HEAP_END 0x300000      // 3MB

// Physical memory manager
void pmm_init(void);
void *pmm_alloc_page(void);
void pmm_free_page(void *page);
void pmm_mark_used(void *page);
void pmm_mark_free(void *page);

// Virtual memory manager (basic)
void vmm_init(void);
void *vmm_alloc(size_t size);
void vmm_free(void *ptr);

// Heap allocator
extern uint32_t heap_allocated;
void *kmalloc(size_t size);
void kfree(void *ptr);

// Utility functions
void memset(void *ptr, int value, size_t size);
void memcpy(void *dest, const void *src, size_t size);

#endif