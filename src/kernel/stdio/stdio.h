#pragma once

#include "types.h"

enum stdio_type {
	none = 0,
	lem1802 = 1
};

void stdio_init(enum stdio_type type);
void stdio_scroll(u16 lines);
void stdio_newline();
void stdio_putc(char c);
void stdio_printc(char c);
void stdio_printf(char* string);
