#pragma once

#include "drivers/lem1802/lem1802.h"
#include "kernel/mm.h"

typedef enum {
    SOFTINT_PUTS,
    SOFTINT_MALLOC,
    SOFTINT_FREE,
    SOFTINT_CLEAR,
    SOFTINT_SIZE,
    __SOFTINT_NB,
} SoftInt;

typedef void (*IntHandler)(u16, u16, u16);

IntHandler *int_handler_allocate(u16 nb_hardware);
void int_handler_activate();
void interrupt1(u16 message, u16 arg_1);
void interrupt2(u16 message, u16 arg_1, u16 arg_2);
// We don't reference interruptHandler() because it shouldn't ever be called directly.
