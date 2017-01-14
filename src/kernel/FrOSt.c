#include "asm.h"
#include "drivers/hardware.h"
#include "interrupt_handler/interrupt_handler.h"
#include "kernel/memory_manager/memory_manager.h"
#include "kernel/panic/panic.h"
#include "kernel/scheduler/scheduler.h"
#include "kernel/stdio/stdio.h"
#include "types.h"

#include "std/stdio.h"
#include "std/stdlib.h"
#include "std/string.h"

#include "usr/console/console.h"

#include "drivers/clock/clock.h"
#include "drivers/iacm/iacm.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/lem1802/lem1802.h"
#include "drivers/mackapar/mackapar.h"

static Driver *drivers[] = { &driver_iacm, &driver_lem1802, &driver_keyboard, &driver_clock, &driver_m35fd, &driver_m525hd };
#define N_DRIVERS (sizeof(drivers) / sizeof(drivers[0]))

int main (void) {
    mm_init ();

    Int_handler *hardware_int_table = int_handler_allocate (asm_hwn ());
    hardware_loop (hardware_int_table, drivers, N_DRIVERS);

    int_handler_activate ();

    if (driver_clock.devices_list.n_devices == 0 || driver_lem1802.devices_list.n_devices == 0 || driver_keyboard.devices_list.n_devices == 0) {
        kpanic ("Error: At least one generic clock, one LEM1802 and one keyboard are needed. Please connect it/them to the DCPU and try again.");
    }

    stdio_init_output (lem1802, &driver_lem1802);
    stdio_init_input (generic_keyboard, &driver_keyboard);

    for (u16 i = 0; i < driver_lem1802.devices_list.n_devices; ++i) {
        stdio_set_current_output (i);
        clear ();
    }

    add_process (&console_main, "console");

    if (driver_lem1802.devices_list.n_devices > 1) {
        for (u16 i = 0; i < driver_lem1802.devices_list.n_devices; ++i) {
            stdio_set_current_output (i);
            printf ("Press 1, 2, ... to load console on corresponding screen.\nThis screen is number: ");
            char *buffer = (char *)malloc (6); // 0xFFFF is only 5 digits in base 10
            itoa (i + 1, buffer);
            printf (buffer);
            free ((u16 *)buffer);
        }

        char c = '\0';
        do {
            c = getc ();
        } while ((u16) (c - '0') > driver_lem1802.devices_list.n_devices);

        for (u16 i = 0; i < driver_lem1802.devices_list.n_devices; ++i) {
            if (i != (u16) (c - '0') - 1) {
                stdio_set_current_output (i);
                clear ();
            }
        }

        stdio_set_current_output ((u16) (c - '0' - 1));
    }

    scheduler_start (&driver_clock);

    if (driver_lem1802.devices_list.n_devices != 0) {
        for (u16 i = 0; i < driver_lem1802.devices_list.n_devices; ++i) {
            stdio_set_current_output (i);
            printf ("You can safely shut down the system.");
        }
    }
    while (1) {
    }
}
