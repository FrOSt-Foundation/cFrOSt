#include "memoryManager.h"
#include "kernel/panic/panic.h"

/*
 * A memory block is setup like this : | OWNER | SIZE | DATA (or FREE if owner is 0xFFFF) |
 */

// Beggining of dynamically allocated memory by the kernel
#define MEMORY_START ((void*) 0x1000)
// End of dynamically allocated memory by the kernel (we don't want to interfere with the kernel stack, do we?)
#define MEMORY_END ((void*) 0xE999)
#define MEMORY_MIN_SIZE 16
#define TOTAL_MEMORY (MEMORY_END - MEMORY_START)
#define MEMORY_OWNER_FREE 0xFFFF

typedef struct {
    u16 size;
    u16 user;
    u16 data[];
} KmallocHeader;

void mm_init() {
    KmallocHeader *p = (KmallocHeader *) MEMORY_START;
    *p = (KmallocHeader){
        .size = TOTAL_MEMORY,
        .user = MEMORY_OWNER_FREE,
    };
}

static u16 max(u16 a, u16 b) {
    return a > b ? a : b;
}

void *kmalloc(u16 owner, u16 size) {
    u16 realSize = max(sizeof(KmallocHeader) + size, MEMORY_MIN_SIZE);

    KmallocHeader *chunk = MEMORY_START;
    while (chunk->user != MEMORY_OWNER_FREE || chunk->size < realSize) {
        if (chunk->size == 0) {
            kpanic("heap corrupted");
        }
        chunk = (KmallocHeader*)(chunk + chunk->size);
        if ((void*)chunk >= MEMORY_END) {
            kpanic("No memory left");
        }
        if (chunk->size - realSize < MEMORY_MIN_SIZE) {
            // make sure to flag too small memory chunks
            chunk->user = MEMORY_OWNER_FREE;
        } else {
            KmallocHeader *other = (chunk + realSize);
            other->size = chunk->size - realSize;
            other->user = MEMORY_OWNER_FREE;
            chunk->size = realSize;
            chunk->user = owner;
        }
    }
    return chunk->data;
}

void kfree(void *addr) {
    if (addr < MEMORY_START || addr > MEMORY_END) {
        return;
    }
    KmallocHeader *chunk = (addr - sizeof(KmallocHeader));
    chunk->user = MEMORY_OWNER_FREE;

    KmallocHeader *other;
    while ((other = (KmallocHeader*)(chunk + chunk->size))
            && other < (KmallocHeader*)MEMORY_END
            && other->user == MEMORY_OWNER_FREE) {
        chunk->size += other->size;
    }
}
