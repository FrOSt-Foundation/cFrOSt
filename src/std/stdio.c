#include "stdio.h"
#include "types.h"
#include "kernel/interruptHandler/interruptHandler.h"

void puts(char* s) {
    interrupt1(SOFTINT_PUTS, (u16)s);
}
