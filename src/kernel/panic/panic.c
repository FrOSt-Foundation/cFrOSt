#include "panic.h"
#include "asm.h"

__attribute__ ((noreturn)) void kpanic (char *data) {
    asm_log ((u16)data);
    // Disable interrupts
    asm_iaq (0);
    asm_ias (0);

    while (true) {
        asm_hlt ();
    }
}
