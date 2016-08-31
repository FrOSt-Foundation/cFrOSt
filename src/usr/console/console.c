#include "console.h"

#include "types.h"
#include "std/stdio.h"
#include "std/stdlib.h"
#include "std/string.h"

#include "programs.h"

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
; // Fixes color syntax in atom-editor, no real use. Sorry :(

static void console_help(u16 UNUSED(n_arguments), char** UNUSED(arguments));
static void console_about(u16 UNUSED(n_arguments), char** UNUSED(arguments));
static void console_echo(u16 n_arguments, char** arguments);
static void console_uptime(u16 UNUSED(n_arguments), char** UNUSED(arguments));
static void console_no_such_command(u16 UNUSED(n_arguments), char** UNUSED(arguments));

static const char* console_commands[4];

static Console_program *console_commands_ptr;

void console_main() {
	console_commands[0] = "help";
	console_commands[1] = "about";
	console_commands[2] = "echo";
	console_commands[3] = "uptime";

	console_commands_ptr = (Console_program *) malloc(__PROGRAMS_SIZE);
	console_commands_ptr[PROGRAM_HELP] = console_help;
	console_commands_ptr[PROGRAM_ABOUT] = console_about;
	console_commands_ptr[PROGRAM_ECHO] = console_echo;
	console_commands_ptr[PROGRAM_UPTIME] = console_uptime;
	console_commands_ptr[PROGRAM_NO_SUCH_COMMAND] = console_no_such_command;

	clear();
	printf("Welcome to FrOSt. Type 'help' if you are lost.");
	moveCursor(0, 11);
	printc('>');
	moveCursor(2, 11);
	printc(CURSOR_CHAR);

	char *inputBuffer = (char*) malloc(INPUT_BUFFER_SIZE + 1); // Whole line, minus the '> ' prompt, plus \0
	u16 inputIndex = 0;

	while(true) {
		char c = getc();
		switch(c) {
			case 0x11: // KEY_RETURN
				// And now we run the program
				moveCursor(inputIndex + 2, 11); // Remove cursor char
				printc(' ');
				scroll(1);
				moveCursor(0, 11);

				u16 n_arguments = 0;
				u16 in_quotes = false;

				// We go through the buffer twice ; once to know how many argument buffers to allocate, and once to allocate them

				for(u16 i = 0; i < INPUT_BUFFER_SIZE; ++i) {
					if(inputBuffer[i] == '"') {
						in_quotes = !in_quotes;
					} else if(inputBuffer[i] == ' ' && !in_quotes) {
						// We find the next argument
						u16 cutLocation = i;
						do {
							cutLocation++;
						} while(inputBuffer[cutLocation] == ' ');

						// And we count it
						if(inputBuffer[cutLocation] != '\0') {
							n_arguments++;
						}
					}
				}

				in_quotes = false;

				char** arguments = (char**) malloc(n_arguments);
				u16 currentArgument = 0;
				for(u16 i = 0; i < INPUT_BUFFER_SIZE; ++i) {
					if(inputBuffer[i] == '"') {
						in_quotes = !in_quotes;
					} else if(inputBuffer[i] == ' ' && !in_quotes) {
						inputBuffer[i] = '\0';

						// We find the next argument
						u16 cutLocation = i;
						do {
							cutLocation++;
						} while(inputBuffer[cutLocation] == ' ');

						// And we store it
						if(inputBuffer[cutLocation] != '\0') {
							arguments[currentArgument] = &inputBuffer[cutLocation];
							currentArgument++;
						}
					}
				}

				u16 commandFound = false;
				for(u16 i = 0; i < __PROGRAMS_SIZE - 1; ++i) {
					if(strcmp(inputBuffer, console_commands[i]) == 0) {
						console_commands_ptr[i](n_arguments, arguments);
						commandFound = true;
						break;
					}
				}

				if(!commandFound)
					console_commands_ptr[PROGRAM_NO_SUCH_COMMAND](0, NULL);

				for(u16 i = 0; i < n_arguments; ++i) {
					free((u16 *) arguments[i]);
				}
				free((u16 *) arguments);

				printf("\n");

				// Clear the buffer for future use
				for(u16 i = 0; i < INPUT_BUFFER_SIZE; ++i) {
					inputBuffer[i] = '\0';
				}
				inputIndex = 0;

				// Restore the prompt
				moveCursor(0, 11);
				printc('>');
				moveCursor(2, 11);
				printc(CURSOR_CHAR);
				break;
			case 0x10: // KEY_BACKSPACE
				if(inputIndex > 0) {
					inputIndex--;
					inputBuffer[inputIndex] = '\0';
					moveCursor(inputIndex + 2, 11);
					printc(' ');
					moveCursor(inputIndex + 2, 11);
					printc(CURSOR_CHAR);
					printc(' ');
				}
				break;
			default:
				if(inputIndex < INPUT_BUFFER_SIZE && c >= 0x20 && c <= 0x7E) {
					inputBuffer[inputIndex] = c;
					moveCursor(inputIndex + 2, 11);
					printc(inputBuffer[inputIndex]);
					printc(CURSOR_CHAR);

					inputIndex++;
				}
		}

	}
}
