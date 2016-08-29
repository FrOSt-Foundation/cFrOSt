#include "stdio.h"
#include "types.h"
#include "kernel/interruptHandler/interruptHandler.h"

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
