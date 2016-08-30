#include "programs.h"
#include "std/stdio.h"

void console_help() {
	printf("Commands: help, about");
}

void console_about() {
	printf("cFrOSt, a program brought to you by l3nn0x, Yamakaky and Azeryfun!");
}

void console_no_such_command() {
	printf("Error: No such command.");
}
