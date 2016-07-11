#pragma once

#include "types.h"
#include "kernel/interruptHandler/interruptHandler.h"

extern u16 monitor, keyboard, clock;

void hardwareLoop(IntHandler *hardware_int_table);
