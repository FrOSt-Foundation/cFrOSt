#pragma once

#include "types.h"

#define CURSOR_CHAR (char)0xDF
#define INPUT_BUFFER_SIZE 29

void console_main ();

typedef void (*Console_program) (u16 n_arguments, char **arguments);

typedef enum {
    PROGRAM_HELP,
    PROGRAM_ABOUT,
    PROGRAM_ECHO,
    PROGRAM_PS,
    PROGRAM_KILL,
    PROGRAM_LSDRIVES,
    PROGRAM_DD,
    PROGRAM_NO_SUCH_COMMAND,
    __PROGRAMS_SIZE,
} programs;
