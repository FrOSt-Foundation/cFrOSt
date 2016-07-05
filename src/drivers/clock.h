#pragma once

#include "types.h"

void clock_set_tickrate(u16 clock, u16 rate);
u16 clock_ticks_since_last_call(u16 clock);
void clock_set_int_msg(u16 clock, u16 msg);
void clock_disable_int(u16 clock);
