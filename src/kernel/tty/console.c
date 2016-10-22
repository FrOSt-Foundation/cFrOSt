#include "kernel/tty/console.h"
#include "kernel/stdio/stdio.h"
#include "types.h"

Terminal *current_term;

void putc_console(char c) {
    Process *p;

    if (!current_term || !(p = current_term->pread)) {
        stdio_putc(c);
        return;
    }

    VConsole *console = p->console;
    if (console->mode == NOT_BUFFERED) {
        stdio_putc(c);
        console->in[0] = c;
        console->term->pread = NULL;
        console->lock = 0;
    } else {
        if (c == 0x10) { // backspace
            if (console->keypos) {
                console->in[console->keypos--] = 0;
                if (console->mode == BUFFERED)
                    stdio_putc(c);
            }
        } else if (c == 0x11) { // new line
            if (console->mode == BUFFERED)
                stdio_putc(c);
            if (console->keypos + 1 < TERMINAL_MAX_BUF_SIZE)
                console->in[console->keypos++] = c;
            console->in[console->keypos] = 0;
            console->term->pread = NULL;
            console->lock = 0;
            console->keypos = 0;
        } else {
            if (console->mode == BUFFERED)
                stdio_putc(c);
            if (console->keypos + 1 < TERMINAL_MAX_BUF_SIZE)
                console->in[console->keypos++] = c;
            else {
                console->in[console->keypos] = 0;
                console->term->pread = NULL;
                console->lock = 0;
                console->keypos = 0;
            }
        }
    }
}
