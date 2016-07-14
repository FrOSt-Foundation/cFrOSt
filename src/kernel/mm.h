#pragma once

#include "types.h"

void memoryManager_init();
u16* memoryManager_malloc(u16 size);
void memoryManager_free(u16* block);
void memoryManager_clear(u16* block);
u16 memoryManager_size(u16* block);
