#include "../include/memory.h"
#include <stdint.h>

uint32_t heap_allocated = 0;

static uint8_t memory_bitmap[BITMAP_SIZE];

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

void pmm_init(void) {
    memset(memory_bitmap, 0, BITMAP_SIZE);

    for (uintptr_t addr = KERNEL_START; addr < KERNEL_END; addr += PAGE_SIZE) {
        pmm_mark_used((void *)addr);
    }

    for (uintptr_t addr = HEAP_START; addr < HEAP_END; addr += PAGE_SIZE) {
        pmm_mark_free((void *)addr);
    }
}

void *pmm_alloc_page(void) {
    static size_t last_alloc_page = 0;
    for (size_t i = 0; i < TOTAL_PAGES; i++) {
        size_t page = (last_alloc_page + i) % TOTAL_PAGES;
        size_t byte = page / 8;
        size_t bit = page % 8;

        if ((memory_bitmap[byte] & (1 << bit)) == 0) {
            memory_bitmap[byte] |= (1 << bit);
            last_alloc_page = page + 1;
            return (void *)(page * PAGE_SIZE);
        }
    }
    return NULL;
}

void pmm_free_page(void *page) {
    uintptr_t addr = (uintptr_t)page;
    size_t page_index = addr / PAGE_SIZE;

    if (page_index >= TOTAL_PAGES) return;

    size_t byte = page_index / 8;
    size_t bit = page_index % 8;

    memory_bitmap[byte] &= ~(1 << bit);
}

void pmm_mark_used(void *page) {
    uintptr_t addr = (uintptr_t)page;
    size_t page_index = addr / PAGE_SIZE;

    if (page_index >= TOTAL_PAGES) return;

    size_t byte = page_index / 8;
    size_t bit = page_index % 8;

    memory_bitmap[byte] |= (1 << bit);
}

void pmm_mark_free(void *page) {
    uintptr_t addr = (uintptr_t)page;
    size_t page_index = addr / PAGE_SIZE;

    if (page_index >= TOTAL_PAGES) return;

    size_t byte = page_index / 8;
    size_t bit = page_index % 8;

    memory_bitmap[byte] &= ~(1 << bit);
}

void vmm_init(void) {
}

typedef struct {
    uint32_t magic;
    size_t size;
    uint8_t is_free;
} heap_block_t;

#define HEAP_MAGIC 0xDEADBEEF

void *kmalloc(size_t size) {
    if (size == 0) return NULL;
    if (size > (HEAP_END - HEAP_START)) return NULL;

    size_t total_size = sizeof(heap_block_t) + size;
    total_size = (total_size + 7) & ~7;

    uint8_t *heap = (uint8_t *)HEAP_START;
    uint8_t *heap_end = (uint8_t *)HEAP_END;

    uint8_t *current = heap;
    while (current + total_size <= heap_end) {
        heap_block_t *block = (heap_block_t *)current;

        if (block->magic == HEAP_MAGIC && block->is_free && block->size >= size) {
            block->is_free = 0;
            return (void *)(current + sizeof(heap_block_t));
        }

        if (block->magic != HEAP_MAGIC) {
            block->magic = HEAP_MAGIC;
            block->size = 0;
            block->is_free = 1;
        }

        if (block->size == 0) {
            if (current + total_size <= heap_end) {
                block->size = size;
                block->is_free = 0;
                heap_allocated += size;
                return (void *)(current + sizeof(heap_block_t));
            }
        }

        current += sizeof(heap_block_t) + block->size;
        current = (uint8_t *)(((uintptr_t)current + 7) & ~7);
    }

    return NULL;
}

void kfree(void *ptr) {
    if (!ptr) return;

    uint8_t *block_start = (uint8_t *)ptr - sizeof(heap_block_t);
    heap_block_t *block = (heap_block_t *)block_start;

    if (block->magic != HEAP_MAGIC) return;
    if (block->is_free) return;

    block->is_free = 1;
    heap_allocated -= block->size;
}

void *vmm_alloc(size_t size) {
    (void)size;
    return NULL;
}

void vmm_free(void *ptr) {
    (void)ptr;
}
