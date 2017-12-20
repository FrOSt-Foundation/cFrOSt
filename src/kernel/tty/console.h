
#ifndef _TTY_H_
#define _TTY_H_

#include "kernel/scheduler/scheduler.h"
#include "types.h"
#define TERMINAL_MAX_BUF_SIZE 16

typedef struct {
    Process *pread; // processus reading on the console
    Process *pwrite; // processus writting on the console
} Terminal;

typedef struct VConsole {
    enum Mode {
        NOT_BUFFERED, // return first character, print on screen
        BUFFERED, // return buffer when \n or max size, print on screen
        FULLY_BUFFERED // return buffer when \n or max size, do not print on screen (password anyone?)
    };
    Terminal *term; // terminal
    char in[TERMINAL_MAX_BUF_SIZE];
    u16 lock; // FIXME: Apparently llvm doesn't like bool
    u16 keypos;
    enum Mode mode;
} VConsole;

extern Terminal *current_term;

void putc_console (char);

#endif /* !_TTY_H_ */
