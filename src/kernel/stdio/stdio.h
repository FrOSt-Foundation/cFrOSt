#pragma once

#include "types.h"
#include "drivers/hardware.h"

typedef enum {
	no_output = 0,
	lem1802 = 1
} Stdio_output_type;

typedef enum {
	no_input = 0,
	generic_keyboard = 1
} Stdio_input_type;

typedef enum {
	ROWS,
	COLS,
	ROW,
	COL,
	__OUTPUTDATA_SIZE
} OutputData;

void stdio_init_output(Stdio_output_type type, Driver* driver);
void stdio_init_input(Stdio_input_type type, Driver* driver);
void stdio_set_current_output(u16 output_number);
void stdio_scroll(u16 lines);
void stdio_newline();
void stdio_putc(char c);
void stdio_printc(char c);
void stdio_printf(char* string);
void stdio_moveCursor(u16 x, u16 y);
void stdio_clear();

char stdio_getc();
