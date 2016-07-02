#pragma once

/*
 * A memory block is setup like this : | OWNER | SIZE | DATA (or FREE if owner is 0xFFFF) |
 */


#define MEMORY_START (u16*) 0x1000 // Beggining of dynamically allocated memory by the kernel
#define MEMORY_END (u16*) 0xE999 // End of dynamically allocated memory by the kernel (we don't want to interfere with the kernel stack, do we?)
#define TOTAL_MEMORY MEMORY_END - MEMORY_START
#define MEMORY_OWNER_FREE 0xFFFF
#define MALLOC_ERROR 0xFFFF

#include "include/types.h"

extern u16 memoryManager_nFree;

void memoryManager_init();
u16* memoryManager_malloc(u16 size);
void memoryManager_free(u16* block);
void memoryManager_clear(u16* block);
u16 memoryManager_size(u16* block);
