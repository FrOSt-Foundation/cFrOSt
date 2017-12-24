#pragma once

#include "types.h"

/*
 * A memory block is setup like this : | OWNER | SIZE | DATA (or FREE if owner is 0xFFFF) |
 */

// Beggining of dynamically allocated memory by the kernel
#define MEMORY_START ((void *)0x2000)
// End of dynamically allocated memory by the kernel (we don't want to interfere with the kernel stack, do we?)
#define MEMORY_END ((void *)0xE999)
#define TOTAL_MEMORY ((u16)MEMORY_END - (u16)MEMORY_START)

#define MEMORY_OWNER_FREE 0xFFFF
#define MEMORY_OWNER_KERNEL 0
#define MEMORY_OWNER_UNKNOWN 0xfffe

typedef struct {
    u16 user;
    u16 size;
    u16 data[];
} Kmalloc_header;

void mm_init (void);
u16 kget_free_memory (void);
void *kmalloc (u16 owner, u16 size);
void kfree (void *block);
void kfree_pid (u16 pid);
