#pragma once

#include "types.h"

void memoryManager_init();
u16* memoryManager_malloc(u16 size);
void memoryManager_free(u16* block);
