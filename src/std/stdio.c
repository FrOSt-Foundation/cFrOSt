#include "stdio.h"
#include "kernel/interrupt_handler/interrupt_handler.h"
#include "types.h"

/*
 * OUTPUT
 */

void printf (char *s) {
    interrupt (SOFTINT_PRINTF, (u16)s, 0, 0);
}

void printc (char c) {
    interrupt (SOFTINT_PRINTC, (u16)c, 0, 0);
}

void scroll (u16 lines) {
    interrupt (SOFTINT_SCROLL, lines, 0, 0);
}

void newline () {
    interrupt (SOFTINT_NEWLINE, 0, 0, 0);
}

void move_cursor (u16 x, u16 y) {
    interrupt (SOFTINT_MOVECURSOR, x, y, 0);
}

void clear () {
    interrupt (SOFTINT_CLEAR, 0, 0, 0);
}


/*
 * INPUT
 */

char getc () {
    char c;
    do {
        interrupt (SOFTINT_GETC, (u16)&c, 0, 0);
    } while (c == '\0');

    return c;
}
