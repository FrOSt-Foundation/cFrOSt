#pragma once

#define CURSOR_CHAR (char) 0xDF
#define INPUT_BUFFER_SIZE 29

void console_main();

typedef void (*Console_program)();

typedef enum {
	PROGRAM_HELP,
	PROGRAM_ABOUT,
	PROGRAM_NO_SUCH_COMMAND,
	__PROGRAMS_SIZE,
} programs;
