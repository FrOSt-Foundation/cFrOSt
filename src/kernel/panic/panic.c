#include "panic.h"

__attribute__ ((noreturn)) void kpanic (char *data) {
    asm_log ((u16)data);
    asm_brk (0);
    asm_iaq (1);
    asm_hlt();
}
