#pragma once

#include "include/types.h"

extern u16 *vram;
extern u16 cursorPos;

void lem1802_init(u16 monitor);
void lem1802_puts(char* s);
