#ifndef INTERRUPTHANDLER_H
#define INTERRUPTHANDLER_H

#include "../../drivers/lem1802/lem1802.h"
#include "../memoryManager/memoryManager.h"

void interruptHandler_init();
void interrupt(u16 message);
// We don't reference interruptHandler() because it shouldn't ever be called directly.

#endif
