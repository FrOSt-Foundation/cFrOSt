#pragma once

#include "asm.h"
#include "kernel/interrupt_handler/interrupt_handler.h"
#include "types.h"

typedef struct {
    u16 n_devices;
    u16 *ids;
    void **data;
} Devices_list;

typedef struct {
    Hardware_info hardware_info;
    void *(*init_function) (u16 id, u16 int_number, Int_handler *int_handler_location);
    void (*destroy_function) (void *data);
    u16 (*update_function) (void *data, u16 arg1, u16 arg2, u16 arg3);

    // Must be initialized with n_devices = 0
    Devices_list devices_list;
} Driver;

void hardware_loop (Int_handler *hardware_int_table, Driver **drivers, u16 n_drivers);
