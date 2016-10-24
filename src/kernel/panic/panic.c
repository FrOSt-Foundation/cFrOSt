#include "panic.h"
#include "kernel/stdio/stdio.h"

extern Driver driver_lem1802;

__attribute__ ((noreturn)) void kpanic (char *data) {
    stdio_init_output (lem1802, &driver_lem1802);
    for (u16 i = 0; i < driver_lem1802.devices_list.n_devices; ++i) {
        stdio_set_current_output (i);
        stdio_clear ();
        stdio_printf ("KERNEL PANIC\n");
        stdio_printf (data);
    }

    asm_log ((u16)data);
    // Disable interrupts
    asm_iaq (0);
    asm_ias (0);

    while (true) {
        asm_hlt ();
    }
}
