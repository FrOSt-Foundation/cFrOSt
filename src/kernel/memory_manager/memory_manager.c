#include "memory_manager.h"
#include "kernel/panic/panic.h"
#include <stdbool.h>

/*
 * A memory block is setup like this : | OWNER | SIZE | DATA (or FREE if owner is 0xFFFF) |
 */

static u16 free_mem = TOTAL_MEMORY;

void mm_init () {
    Kmalloc_header *p = (Kmalloc_header *)MEMORY_START;
    *p = (Kmalloc_header){
        .user = MEMORY_OWNER_FREE,
        .size = TOTAL_MEMORY,
    };
}

void *kmalloc (u16 owner, u16 size) {

    u16 real_size = size + 2;

    free_mem -= real_size;

    if (size > free_mem) {
        kpanic ("Out of memory");
    }

    Kmalloc_header *chunk = MEMORY_START;
    while (true) {
        if ((void *)chunk >= MEMORY_END) {
            kpanic ("Out of memory");
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

        chunk = (Kmalloc_header *)((u16)chunk + chunk->size + 2);
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
    Kmalloc_header *chunk = (addr - sizeof (Kmalloc_header));
    chunk->user = MEMORY_OWNER_FREE;

    Kmalloc_header *other;
    do {
        other = (Kmalloc_header *)((u16)chunk + chunk->size + 2);
        if (other->user == MEMORY_OWNER_FREE) {
            chunk->size += other->size + 2;
        }
    } while (other < (Kmalloc_header *)MEMORY_END && other->user == MEMORY_OWNER_FREE);
}

void kfree_pid (u16 pid) {
    Kmalloc_header *chunk = (Kmalloc_header *)MEMORY_START;

    u16 s = chunk->size;

    if (chunk->user == pid) {
        kfree (chunk);
    }

    chunk += s + 2;
}
