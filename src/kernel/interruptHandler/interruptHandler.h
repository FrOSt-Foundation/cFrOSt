#ifndef INTERRUPTHANDLER_H
#define INTERRUPTHANDLER_H

void interruptHandler_init();
void interrupt(u16 message, u16 argument);
// We don't reference interruptHandler() because it shouldn't ever be called directly.

#endif
