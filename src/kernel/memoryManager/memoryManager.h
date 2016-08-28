#pragma once

#include "types.h"

#define MEMORY_OWNER_KERNEL 0
#define MEMORY_OWNER_UNKNOWN 0xfffe

void mm_init();
void *kmalloc(u16 owner, u16 size);
void kfree(void* block);
