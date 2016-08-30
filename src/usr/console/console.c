#include "console.h"

#include "types.h"
#include "std/stdio.h"
#include "std/stdlib.h"
#include "std/string.h"

#include "programs.h"

static void console_help();
static void console_about();
static void console_no_such_command();

static const char* console_commands[2];

static Console_program *console_commands_ptr;

void console_main() {
	console_commands[0] = "help";
	console_commands[1] = "about";

	console_commands_ptr = (Console_program *) malloc(__PROGRAMS_SIZE);
	console_commands_ptr[PROGRAM_HELP] = console_help;
	console_commands_ptr[PROGRAM_ABOUT] = console_about;
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
				// We visually clear the prompt first
				moveCursor(0, 11);
				for(u16 i = 0; i < INPUT_BUFFER_SIZE + 2; ++i) {
					printc(' ');
				}

				// And now we run the program
				moveCursor(0, 11);

				u16 commandFound = false;
				for(u16 i = 0; i < __PROGRAMS_SIZE - 1; ++i) {
					if(strcmp(inputBuffer, console_commands[i]) == 0) {
						console_commands_ptr[i]();
						commandFound = true;
						break;
					}
				}

				if(!commandFound)
					console_commands_ptr[PROGRAM_NO_SUCH_COMMAND]();

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
