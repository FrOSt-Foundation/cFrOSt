#pragma once

#include "drivers/lem1802/lem1802.h"
#include "kernel/memoryManager/memoryManager.h"

void interruptHandler_init();
void interrupt(u16 message);
// We don't reference interruptHandler() because it shouldn't ever be called directly.
