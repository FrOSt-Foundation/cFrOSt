#pragma once

#include "include/types.h"

void memcpy(const void *from, void *to, u16 size);
u16* malloc(u16 size);
void free(u16* block);
void mclear(u16* block);
u16 msize(u16* block);
u16* realloc(u16* block, u16 newSize);
