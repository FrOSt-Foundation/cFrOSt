#pragma once

#include "types.h"

void mm_init();
u16* kmalloc(u16 size);
void kfree(u16* block);
