#include "memoryManager.h"

u16 memoryManager_nFree = TOTAL_MEMORY;

void memoryManager_init() {
    u16* p = (u16 *) MEMORY_START;
    *p = MEMORY_OWNER_FREE;
    *(p + 1) = TOTAL_MEMORY;
}

u16* memoryManager_malloc(u16 size) {
    if(size + 2 > memoryManager_nFree) // We use size + 2 because we also have to store the header (owner and size)
        return (u16*) MALLOC_ERROR;
    if(size == 0) // u wot m8
        return (u16*) MALLOC_ERROR;

    memoryManager_nFree -= size;

    for(u16* p = MEMORY_START ; p < MEMORY_END; p += *(p + 1) + 2) {
        if(*p == MEMORY_OWNER_FREE && *(p + 1) >= size) { // If we have a big enough free block
            *p = 0; // TODO: Update ownership system when we have multiple processes support
            *(p + 1) = size;

            // We clear the memory block before usage
            for(u16 i = 0; i < size; ++i) {
                *(p + i + 2) = 0;
            }

            return p + 2;
        }
    }

    return (u16*) MALLOC_ERROR; // Could not find a big enough block
}

void memoryManager_free(u16* block) {
    if(block < MEMORY_START || block > MEMORY_END)
        return;

    // We find the nearest block from the bottom
    u16* p = (u16 *) MEMORY_START;
    while(p + *(p + 1) + 2 < block) {
        p += *(p + 1) + 2;
    }

    *p = MEMORY_OWNER_FREE;
    memoryManager_nFree += *(p + 1);
}

void memoryManager_clear(u16* block) {
    if(block < MEMORY_START || block > MEMORY_END)
        return;

    // We find the nearest block from the bottom
    u16* p = (u16 *) MEMORY_START;
    while(p + *(p + 1) + 2 < block) {
        p += *(p + 1) + 2;
    }

    for(u16 i = 0; i < *(p + 1); ++i) {
        *(p + i + 2) = 0;
    }
}

u16 memoryManager_size(u16* block) {
    if(block < MEMORY_START || block > MEMORY_END)
        return MALLOC_ERROR;

    // We find the nearest block from the bottom
    u16* p = (u16 *) MEMORY_START;
    while(p + *(p + 1) + 2 < block) {
        p += *(p + 1) + 2;
    }

    return *(p + 1);
}
