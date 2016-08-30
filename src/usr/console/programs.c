#include "programs.h"
#include "std/stdio.h"

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
; // Fixes color syntax in atom-editor, no real use. Sorry :(

void console_help(u16 UNUSED(n_arguments), char** UNUSED(arguments)) {
	printf("Commands: help, about, echo");
}

void console_about(u16 UNUSED(n_arguments), char** UNUSED(arguments)) {
	printf("cFrOSt, a program brought to you by l3nn0x, Yamakaky and Azeryfun!");
}

void console_echo(u16 n_arguments, char** arguments) {
	if(n_arguments != 0) {
		for(u16 i = 0; i < n_arguments; ++i) {
			printf(arguments[i]);
			printc(' ');
		}
	}
}

void console_no_such_command(u16 UNUSED(n_arguments), char** UNUSED(arguments)) {
	printf("Error: No such command.");
}
