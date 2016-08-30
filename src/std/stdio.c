#include "stdio.h"
#include "types.h"
#include "kernel/interruptHandler/interruptHandler.h"

/*
 * OUTPUT
 */

void printf(char* s) {
	interrupt1(SOFTINT_PRINTF, (u16)s);
}

void printc(char c) {
	interrupt1(SOFTINT_PRINTC, (u16) c);
}

void scroll(u16 lines) {
	interrupt1(SOFTINT_SCROLL, lines);
}

void newline() {
	interrupt1(SOFTINT_NEWLINE, 0);
}

void moveCursor(u16 x, u16 y) {
	interrupt2(SOFTINT_MOVECURSOR, x, y);
}

void clear() {
	interrupt1(SOFTINT_CLEAR, 0);
}


/*
 * INPUT
 */

char getc() {
	u16 c;
	interrupt1(SOFTINT_GETC, (u16) &c);

	return c;
}
