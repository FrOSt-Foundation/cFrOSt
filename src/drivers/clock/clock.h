#pragma once

#include "types.h"

typedef enum {
	CLOCK_SET_TICKRATE,
	CLOCK_TICKS_SINCE_LAST_CALL,
	CLOCK_SET_INT_MSG
} Clock_message;

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
; // Fixes color syntax in atom-editor, no real use. Sorry :(

typedef struct Clock_driverData Clock_driverData;

void* clock_init(u16 clock, u16 int_number, IntHandler* int_handler_location);
void clock_destroy(void* UNUSED(data));
u16 clock_update_function(void* data, u16 message, u16 arg1, u16 arg2);

void clock_intHandler(u16 message, u16 arg1, u16 arg2, u16 arg3);

void clock_set_tickrate(Clock_driverData* data, u16 rate);
u16 clock_ticks_since_last_call(Clock_driverData* data);
void clock_set_int_msg(Clock_driverData* data, u16 msg);
