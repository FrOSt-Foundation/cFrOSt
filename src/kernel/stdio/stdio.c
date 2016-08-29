#include "stdio.h"

#include "std/stdlib.h"
#include "types.h"
#include "drivers/lem1802/lem1802.h"

static u16 isInitialized = false;
static enum stdio_type type = 0;
static u16 rows = 0;
static u16 cols = 0;
static u16 row = 0;
static u16 col = 0;

// You can init stdio with whatever device you want!
void stdio_init(enum stdio_type t) {
	type = t;

	switch(t) {
		case lem1802:
			rows = 12;
			cols = 32;
			cursorPos = 0;
			isInitialized = true;
			return;
		case none:
			break;
	}
}

void stdio_scroll(u16 lines) {
	for(u16 line = 0; line < lines; ++line) {
		switch(type) {
			case lem1802:
				lem1802_scroll();
				break;
			case none:
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
	stdio_putc(c);

	if(col == cols - 1) {
		stdio_newline();
	} else {
		col++;
	}
}

void stdio_putc(char c) {
	switch(type) {
		case lem1802:
			lem1802_putc(c, row * 32 + col);
			break;
		case none:
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
