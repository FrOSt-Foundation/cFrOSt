#include "memory_manager.h"
#include "kernel/panic/panic.h"
#include <stdbool.h>

/*
 * A memory block is setup like this : | OWNER | SIZE | DATA (or FREE if owner is 0xFFFF) |
 */

static u16 free_mem = TOTAL_MEMORY;

static Kmalloc_header *next (Kmalloc_header *current) {
    return (Kmalloc_header *)((u16)current + current->size + sizeof (Kmalloc_header));
}

static Kmalloc_header *get_header (void *addr) {
    return (Kmalloc_header *)((u16 *)addr - sizeof (Kmalloc_header));
}

static u16 get_real_size (u16 size) {
    return size + sizeof (Kmalloc_header);
}

void mm_init () {
    Kmalloc_header *p = MEMORY_START;
    *p = (Kmalloc_header){
        .user = MEMORY_OWNER_FREE,
        .size = TOTAL_MEMORY,
    };
}

u16 kget_free_memory (void) {
    return free_mem;
}

void *kmalloc (u16 owner, u16 size) {
    u16 real_size = get_real_size (size);

    free_mem -= real_size;

    if (size > free_mem) {
        kpanic ("Out of memory");
    }

    Kmalloc_header *chunk = MEMORY_START;
    while (true) {
        if ((void *)chunk >= MEMORY_END) {
            kpanic ("Out of memory (please make sure no memory corrruption occured)");
        }

        if (chunk->user == MEMORY_OWNER_FREE && chunk->size == real_size) {
            chunk->user = owner;
            break;
        } else if (chunk->user == MEMORY_OWNER_FREE && chunk->size > real_size) {
            // We split the chunk in two
            chunk->data[size] = MEMORY_OWNER_FREE;
            chunk->data[size + 1] = chunk->size - real_size;

            // And now it's ours
            chunk->user = owner;
            chunk->size = size;

            break;
        }

        chunk = next (chunk);
    }

    for (u16 i = 0; i < size; ++i) {
        chunk->data[i] = 0;
    }

    return chunk->data;
}

void kfree (void *addr) {
    if (addr < MEMORY_START || addr > MEMORY_END) {
        return;
    }
    Kmalloc_header *chunk = get_header (addr);
    chunk->user = MEMORY_OWNER_FREE;

    free_mem += chunk->size;

    Kmalloc_header *other;
    do {
        other = next (chunk);
        if (other->user == MEMORY_OWNER_FREE) {
            chunk->size += get_real_size (other->size);
            free_mem += 2;
        }
    } while (other < (Kmalloc_header *)MEMORY_END && other->user == MEMORY_OWNER_FREE);
}

void kfree_pid (u16 pid) {
    Kmalloc_header *chunk = MEMORY_START;

    do {
        if (chunk->user == pid)
            kfree (chunk);
        chunk = next (chunk);
    } while (chunk < (Kmalloc_header *)MEMORY_END);
}
