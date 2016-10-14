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

Driver driver_iacm;
Driver driver_lem1802;
Driver driver_keyboard;
Driver driver_clock;

#define N_DRIVERS 4
static Driver *drivers[] = { &driver_iacm, &driver_lem1802, &driver_keyboard, &driver_clock };

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

Driver driver_iacm = (Driver){
    .hardware_info = (Hardware_info){.hardware_id_a = 0xdacc,
                                     .hardware_id_b = 0x11e0,
                                     .hardware_version = 0x0004,
                                     .manufacturer_a = 0x0000, // Vendor code is defined as "(Various)"
                                     .manufacturer_b = 0x0000 },
    .update_function = iacm_update_function,
    .init_function = iacm_init,
    .destroy_function = iacm_destroy,
    .devices_list = (Devices_list){.n_devices = 0 }
};

Driver driver_lem1802 = (Driver){
    .hardware_info = (Hardware_info){.hardware_id_a = 0xf615,
                                     .hardware_id_b = 0x7349,
                                     .hardware_version = 0x1802,
                                     .manufacturer_a = 0x8b36,
                                     .manufacturer_b = 0x1c6c },
    .update_function = lem1802_update_function,
    .init_function = lem1802_init,
    .destroy_function = lem1802_destroy,
    .devices_list = (Devices_list){.n_devices = 0 }
};

Driver driver_keyboard = (Driver){
    .hardware_info = (Hardware_info){.hardware_id_a = 0x7406,
                                     .hardware_id_b = 0x30cf,
                                     .hardware_version = 1,
                                     .manufacturer_a = 0x8b36,
                                     .manufacturer_b = 0x1c6c },
    .update_function = keyboard_update_function,
    .init_function = keyboard_init,
    .destroy_function = keyboard_destroy,
    .devices_list = (Devices_list){.n_devices = 0 }
};

Driver driver_clock = (Driver){
    .hardware_info = (Hardware_info){.hardware_id_a = 0xb402,
                                     .hardware_id_b = 0x12d0,
                                     .hardware_version = 1,
                                     .manufacturer_a = 0x8b36,
                                     .manufacturer_b = 0x1c6c },
    .update_function = clock_update_function,
    .init_function = clock_init,
    .destroy_function = clock_destroy,
    .devices_list = (Devices_list){.n_devices = 0 }
};
