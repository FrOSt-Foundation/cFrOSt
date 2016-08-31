#pragma once

#define CURSOR_CHAR (char) 0xDF
#define INPUT_BUFFER_SIZE 29

void console_main();

typedef void (*Console_program)(u16 n_arguments, char** arguments);

typedef enum {
	PROGRAM_HELP,
	PROGRAM_ABOUT,
	PROGRAM_ECHO,
	PROGRAM_UPTIME,
	PROGRAM_NO_SUCH_COMMAND,
	__PROGRAMS_SIZE,
} programs;
