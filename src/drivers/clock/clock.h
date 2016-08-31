#pragma once

#include "types.h"

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
; // Fixes color syntax in atom-editor, no real use. Sorry :(

typedef struct Clock_driverData Clock_driverData;

void* clock_init(u16 clock);
void clock_destroy(void* UNUSED(data));
void clock_update_function(void* data, u16 message, u16 arg1, u16 arg2);

void clock_set_tickrate(Clock_driverData* data, u16 rate);
u16 clock_ticks_since_last_call(Clock_driverData* data);
void clock_set_int_msg(Clock_driverData* data, u16 msg);
