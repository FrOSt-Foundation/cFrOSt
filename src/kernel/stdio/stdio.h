#pragma once

#include "types.h"

enum stdio_output_type {
	no_output = 0,
	lem1802 = 1
};

enum stdio_input_type {
	no_input = 0,
	generic_keyboard = 1
};

void stdio_init_output(enum stdio_output_type type);
void stdio_init_input(enum stdio_input_type type);
void stdio_scroll(u16 lines);
void stdio_newline();
void stdio_putc(char c);
void stdio_printc(char c);
void stdio_printf(char* string);
void stdio_moveCursor(u16 x, u16 y);
void stdio_clear();

char stdio_getc();
