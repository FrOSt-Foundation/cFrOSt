#include "stdio.h"

#include "std/stdlib.h"
#include "types.h"
#include "drivers/lem1802/lem1802.h"
#include "drivers/keyboard/keyboard.h"

static enum stdio_output_type output_type = no_output;
static enum stdio_input_type input_type = no_input;
static u16 rows = 0;
static u16 cols = 0;
static u16 row = 0;
static u16 col = 0;

// You can init stdio with whatever output device you want!
void stdio_init_output(enum stdio_output_type t) {
	output_type = t;

	switch(t) {
		case lem1802:
			rows = 12;
			cols = 32;
			cursorPos = 0;
			return;
		case no_output:
			break;
	}
}

void stdio_init_input(enum stdio_input_type t) {
	input_type = t;

	switch(t) {
		case generic_keyboard:
			keyboard_clear_buffer();
			return;
		case no_output:
			break;
	}
}

void stdio_scroll(u16 lines) {
	for(u16 line = 0; line < lines; ++line) {
		switch(output_type) {
			case lem1802:
				lem1802_scroll();
				break;
			case no_output:
				break;
		}
	}
}

// Move cursor to a new row, at the first column.
void stdio_newline() {
	col = 0;
	if(row == rows - 1) {
		stdio_scroll(1);
	} else {
		row++;
	}
}

void stdio_printc(char c) {
	if(col == cols)
		stdio_newline();

	stdio_putc(c);

	col++;
}

void stdio_putc(char c) {
	switch(output_type) {
		case lem1802:
			lem1802_putc(c, row * 32 + col);
			break;
		case no_output:
			break;
	}
}

// Print a "formatted" string (only supports newline '\n' for now)
void stdio_printf(char* string) {
	char* p = string;
	while(*p != '\0') {
		switch(*p) {
			case 0xA:
				stdio_newline();
				break;
			default:
				stdio_printc(*p);
		}

		p++;
	}
}

void stdio_moveCursor(u16 x, u16 y) {
	if(x < cols)
		col = x;
	if(y < rows)
		row = y;
}

void stdio_clear() {
	row = 0;
	col = 0;

	switch(output_type) {
		case lem1802:
			lem1802_clear();
			break;
		case no_output:
			break;
	}
}

char stdio_getc() {
	char c = '\0';
	switch (input_type) {
		case generic_keyboard:
			c = keyboard_getc();

			break;
		case no_input:
			break;
	}

	return c;
}
