#include "console.h"

#include "std/stdio.h"
#include "std/stdlib.h"
#include "std/string.h"
#include "types.h"
#include <stdbool.h>

#include "programs.h"

#define CURSOR_CHAR (char)0xDF
#define INPUT_BUFFER_SIZE 29

typedef void (*ProgramMain) (u16 n_arguments, char **arguments);

typedef struct {
    const char *name;
    ProgramMain main;
} Program;

static const Program builtins[] = {
    {
    .name = "help",
    .main = console_help,
    },
    {
    .name = "about",
    .main = console_about,
    },
    {
    .name = "echo",
    .main = console_echo,
    },
    {
    .name = "mem",
    .main = console_mem,
    },
    {
    .name = "ps",
    .main = console_ps,
    },
    {
    .name = "kill",
    .main = console_kill,
    },
    {
    .name = "lsdrives",
    .main = console_lsdrives,
    },
    {
    .name = "dd",
    .main = console_dd,
    },
    {
    .name = "mkfs.bbfs",
    .main = console_mkfs_bbfs,
    },
    {
    .name = "ls",
    .main = console_ls,
    },
};

static const u16 NB_PROGRAMS = sizeof (builtins);

void console_main () {
    clear ();
    printf ("Welcome to FrOSt. Type 'help' if you are lost.");
    move_cursor (0, 11);
    printc ('>');
    move_cursor (2, 11);
    printc (CURSOR_CHAR);

    char *input_buffer = (char *)malloc (INPUT_BUFFER_SIZE + 1); // Whole line, minus the '> ' prompt, plus \0
    u16 input_index = 0;

    while (true) {
        char c = getc ();
        switch (c) {
            case 0x11: // KEY_RETURN
                // And now we run the program
                move_cursor (input_index + 2, 11); // Remove cursor char
                printc (' ');
                scroll (1);
                move_cursor (0, 11);

                u16 n_arguments = 0;
                u16 in_quotes = false;

                // We go through the buffer twice ; once to know how many argument buffers to allocate, and once to allocate them

                for (u16 i = 0; i < INPUT_BUFFER_SIZE; ++i) {
                    if (input_buffer[i] == '"') {
                        in_quotes = !in_quotes;
                    } else if (input_buffer[i] == ' ' && !in_quotes) {
                        // We find the next argument
                        u16 cut_location = i;
                        do {
                            cut_location++;
                        } while (input_buffer[cut_location] == ' ');

                        // And we count it
                        if (input_buffer[cut_location] != '\0') {
                            n_arguments++;
                        }
                    }
                }

                in_quotes = false;

                char **arguments = (char **)malloc (n_arguments);
                u16 current_argument = 0;
                for (u16 i = 0; i < INPUT_BUFFER_SIZE; ++i) {
                    if (input_buffer[i] == '"') {
                        in_quotes = !in_quotes;
                    } else if (input_buffer[i] == ' ' && !in_quotes) {
                        input_buffer[i] = '\0';

                        // We find the next argument
                        u16 cut_location = i;
                        do {
                            cut_location++;
                        } while (input_buffer[cut_location] == ' ');

                        // And we store it
                        if (input_buffer[cut_location] != '\0') {
                            arguments[current_argument] = &input_buffer[cut_location];
                            current_argument++;
                        }
                    }
                }

                u16 command_found = false;
                for (u16 i = 0; i < NB_PROGRAMS - 1; ++i) {
                    if (strcmp (input_buffer, builtins[i].name) == 0) {
                        builtins[i].main (n_arguments, arguments);
                        command_found = true;
                        break;
                    }
                }

                if (!command_found)
                    console_no_such_command (0, NULL);

                free ((u16 *)arguments);

                printf ("\n");

                // Clear the buffer for future use
                for (u16 i = 0; i < INPUT_BUFFER_SIZE; ++i) {
                    input_buffer[i] = '\0';
                }
                input_index = 0;

                // Restore the prompt
                move_cursor (0, 11);
                printc ('>');
                move_cursor (2, 11);
                printc (CURSOR_CHAR);
                break;
            case 0x10: // KEY_BACKSPACE
                if (input_index > 0) {
                    input_index--;
                    input_buffer[input_index] = '\0';
                    move_cursor (input_index + 2, 11);
                    printc (' ');
                    move_cursor (input_index + 2, 11);
                    printc (CURSOR_CHAR);
                    printc (' ');
                }
                break;
            default:
                if (input_index < INPUT_BUFFER_SIZE && c >= 0x20 && c <= 0x7E) {
                    input_buffer[input_index] = c;
                    move_cursor (input_index + 2, 11);
                    printc (input_buffer[input_index]);
                    printc (CURSOR_CHAR);

                    input_index++;
                }
        }
    }
}
