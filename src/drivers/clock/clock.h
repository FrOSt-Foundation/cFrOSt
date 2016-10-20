#pragma once

#include "drivers/hardware.h"
#include "kernel/interrupt_handler/interrupt_handler.h"
#include "kernel/interrupt_handler/interrupt_handler.h"
#include "types.h"

typedef enum {
    CLOCK_SET_TICKRATE,
    CLOCK_TICKS_SINCE_LAST_CALL,
    CLOCK_SET_INT_MSG
} Clock_message;

typedef struct {
    u16 clock;
} Clock_driver_data;

extern Driver driver_clock;

void *clock_init (u16 clock, u16 int_number, Int_handler *int_handler_location);
void clock_destroy (void *UNUSED (data));
u16 clock_update_function (void *data, u16 message, u16 arg1, u16 arg2);

void clock_int_handler (u16 message, u16 arg1, u16 arg2, u16 arg3);

void clock_set_tickrate (Clock_driver_data *data, u16 rate);
u16 clock_ticks_since_last_call (Clock_driver_data *data);
void clock_set_int_msg (Clock_driver_data *data, u16 msg);
