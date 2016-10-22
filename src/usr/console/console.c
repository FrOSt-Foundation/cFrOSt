#include "console.h"

#include "std/stdio.h"
#include "std/stdlib.h"
#include "std/string.h"
#include "types.h"
#include "types.h"
#include <stdbool.h>

#include "programs.h"

static const char *console_commands[__PROGRAMS_SIZE];

static Console_program *console_commands_ptr;

void console_main (void) __attribute__((noreturn));

void console_main () {
    console_commands[0] = "help";
    console_commands[1] = "about";
    console_commands[2] = "echo";
    console_commands[3] = "ps";
    console_commands[4] = "kill";

    console_commands_ptr = (Console_program *)malloc (__PROGRAMS_SIZE);
    console_commands_ptr[PROGRAM_HELP] = console_help;
    console_commands_ptr[PROGRAM_ABOUT] = console_about;
    console_commands_ptr[PROGRAM_ECHO] = console_echo;
    console_commands_ptr[PROGRAM_PS] = console_ps;
    console_commands_ptr[PROGRAM_KILL] = console_kill;
    console_commands_ptr[PROGRAM_NO_SUCH_COMMAND] = console_no_such_command;

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
                for (u16 i = 0; i < __PROGRAMS_SIZE - 1; ++i) {
                    if (strcmp (input_buffer, console_commands[i]) == 0) {
                        console_commands_ptr[i](n_arguments, arguments);
                        command_found = true;
                        break;
                    }
                }

                if (!command_found)
                    console_commands_ptr[PROGRAM_NO_SUCH_COMMAND](0, NULL);

                for (u16 i = 0; i < n_arguments; ++i) {
                    free ((u16 *)arguments[i]);
                }
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
