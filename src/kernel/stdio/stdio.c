#include "stdio.h"

#include "std/stdlib.h"
#include "types.h"
#include "drivers/lem1802/lem1802.h"
#include "drivers/keyboard/keyboard.h"

static Stdio_output_type output_type = no_output;
static Driver* output_driver;
static Stdio_input_type input_type = no_input;

static u16** outputData;
static u16 currentOutput;

// You can init stdio with whatever output device you want!
void stdio_init_output(Stdio_output_type t, Driver* d) {
	output_type = t;

	if(outputData != 0) {
		for(u16 i = 0; i < output_driver->devicesList.nDevices; ++i) {
			kfree(outputData[i]);
		}
		kfree(outputData);
	}

	output_driver = d;

	outputData = kmalloc(0, d->devicesList.nDevices);

	currentOutput = 0;

	switch(t) {
		case lem1802:
			for(u16 i = 0; i < d->devicesList.nDevices; ++i) {
				outputData[i] = kmalloc(0, __OUTPUTDATA_SIZE);
				outputData[i][ROWS] = 12;
				outputData[i][COLS] = 32;
				outputData[i][ROW] = 0;
				outputData[i][COL] = 0;
			}
			return;
		case no_output:
			break;
	}
}

void stdio_init_input(Stdio_input_type t) {
	input_type = t;

	switch(t) {
		case generic_keyboard:
			keyboard_clear_buffer();
			return;
		case no_output:
			break;
	}
}

void stdio_set_current_output(u16 output_number) {
	currentOutput = output_number;
}

void stdio_scroll(u16 lines) {
	for(u16 line = 0; line < lines; ++line) {
		switch(output_type) {
			case lem1802:
				lem1802_scroll(output_driver->devicesList.data[currentOutput]);
				break;
			case no_output:
				break;
		}
	}
}

// Move cursor to a new row, at the first column.
void stdio_newline() {
	outputData[currentOutput][COL] = 0;
	if(outputData[currentOutput][ROW] == outputData[currentOutput][ROWS] - 1) {
		stdio_scroll(1);
	} else {
		outputData[currentOutput][ROW]++;
	}
}

void stdio_printc(char c) {
	if(outputData[currentOutput][COL] == outputData[currentOutput][COLS])
		stdio_newline();

	stdio_putc(c);

	outputData[currentOutput][COL]++;
}

void stdio_putc(char c) {
	switch(output_type) {
		case lem1802:
			lem1802_putc(output_driver->devicesList.data[currentOutput], c, outputData[currentOutput][ROW] * 32 + outputData[currentOutput][COL]);
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
	if(x < outputData[currentOutput][COLS])
		outputData[currentOutput][COL] = x;
	if(y < outputData[currentOutput][ROWS])
		outputData[currentOutput][ROW] = y;
}

void stdio_clear() {
	outputData[currentOutput][ROW] = 0;
	outputData[currentOutput][COL] = 0;

	switch(output_type) {
		case lem1802:
			lem1802_clear(output_driver->devicesList.data[currentOutput]);
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
