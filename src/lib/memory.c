#include "../include/memory.h"

// Physical memory bitmap
static uint8_t memory_bitmap[BITMAP_SIZE];

// Heap management
#define HEAP_SIZE (HEAP_END - HEAP_START)
static uint8_t heap_bitmap[HEAP_SIZE / PAGE_SIZE / 8];
static uint32_t heap_allocated = 0;

// Utility functions
void memset(void *ptr, int value, size_t size) {
    uint8_t *p = (uint8_t *)ptr;
    for (size_t i = 0; i < size; i++) {
        p[i] = (uint8_t)value;
    }
}

void memcpy(void *dest, const void *src, size_t size) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    for (size_t i = 0; i < size; i++) {
        d[i] = s[i];
    }
}

// Physical Memory Manager
void pmm_init(void) {
    // Clear bitmap
    memset(memory_bitmap, 0, BITMAP_SIZE);

    // Mark kernel memory as used
    for (uint32_t addr = KERNEL_START; addr < KERNEL_END; addr += PAGE_SIZE) {
        pmm_mark_used((void *)addr);
    }

    // Mark heap area as free initially
    for (uint32_t addr = HEAP_START; addr < HEAP_END; addr += PAGE_SIZE) {
        pmm_mark_free((void *)addr);
    }
}

void *pmm_alloc_page(void) {
    for (size_t i = 0; i < TOTAL_PAGES; i++) {
        size_t byte = i / 8;
        size_t bit = i % 8;

        if ((memory_bitmap[byte] & (1 << bit)) == 0) {
            // Page is free
            memory_bitmap[byte] |= (1 << bit);
            return (void *)(i * PAGE_SIZE);
        }
    }
    return NULL; // No free pages
}

void pmm_free_page(void *page) {
    uint32_t addr = (uint32_t)page;
    size_t page_index = addr / PAGE_SIZE;

    if (page_index >= TOTAL_PAGES) return;

    size_t byte = page_index / 8;
    size_t bit = page_index % 8;

    memory_bitmap[byte] &= ~(1 << bit);
}

void pmm_mark_used(void *page) {
    uint32_t addr = (uint32_t)page;
    size_t page_index = addr / PAGE_SIZE;

    if (page_index >= TOTAL_PAGES) return;

    size_t byte = page_index / 8;
    size_t bit = page_index % 8;

    memory_bitmap[byte] |= (1 << bit);
}

void pmm_mark_free(void *page) {
    uint32_t addr = (uint32_t)page;
    size_t page_index = addr / PAGE_SIZE;

    if (page_index >= TOTAL_PAGES) return;

    size_t byte = page_index / 8;
    size_t bit = page_index % 8;

    memory_bitmap[byte] &= ~(1 << bit);
}

// Virtual Memory Manager (basic identity mapping for now)
void vmm_init(void) {
    // For now, just initialize - we'll implement paging later
}

// Simple heap allocator
void *kmalloc(size_t size) {
    if (size == 0) return NULL;

    // Align size to page boundary
    size_t pages_needed = (size + PAGE_SIZE - 1) / PAGE_SIZE;

    for (size_t i = 0; i < HEAP_SIZE / PAGE_SIZE; i++) {
        size_t byte = i / 8;
        size_t bit = i % 8;

        // Check if we have enough consecutive free pages
        int found = 1;
        for (size_t j = 0; j < pages_needed; j++) {
            size_t check_byte = (i + j) / 8;
            size_t check_bit = (i + j) % 8;

            if (check_byte >= sizeof(heap_bitmap) ||
                (heap_bitmap[check_byte] & (1 << check_bit))) {
                found = 0;
                break;
            }
        }

        if (found) {
            // Mark pages as used
            for (size_t j = 0; j < pages_needed; j++) {
                size_t check_byte = (i + j) / 8;
                size_t check_bit = (i + j) % 8;
                heap_bitmap[check_byte] |= (1 << check_bit);
            }

            heap_allocated += pages_needed * PAGE_SIZE;
            return (void *)(HEAP_START + i * PAGE_SIZE);
        }
    }

    return NULL; // No memory available
}

void kfree(void *ptr) {
    if (!ptr) return;

    uint32_t addr = (uint32_t)ptr;
    if (addr < HEAP_START || addr >= HEAP_END) return;

    size_t page_index = (addr - HEAP_START) / PAGE_SIZE;

    size_t byte = page_index / 8;
    size_t bit = page_index % 8;

    // For now, just mark the first page as free
    // TODO: Implement proper size tracking
    heap_bitmap[byte] &= ~(1 << bit);
    heap_allocated -= PAGE_SIZE;
}

void *vmm_alloc(size_t size) {
    // Stub - implement later
    return NULL;
}

void vmm_free(void *ptr) {
    // Stub - implement later
}