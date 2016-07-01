void interruptHandler_init() {
    __asm("IAS ____ptr_interruptHandler");
}

#include "../../std/string.h"

void interrupt(u16 message, u16 argument) {
    u16* p = (u16*) 0xF000;
    *p = message;

    *(p + 1) = argument;
    __asm("set PUSH, B \n SET B, [0xF001] \n int [0xF000] \n set B, POP");
}

/*
 *  The most significant eight bits of the interrupt message define its class. The following eight bits define the interrupt function within that class.
 *
 *  Interrupt (class then function) :
 *    * 0x00: Fallback I/O. Initialized right after FrOSt and not dependant on module loading, allows the kernel to output messages at any time.
 *      - 0x00: puts
 */

void interruptHandler() {
    __asm(":____ptr_interruptHandler");
    __asm volatile("SET [0xF000], A");

    u16* p = (u16*) 0xF000;
    u16 interruptMessage = *p;
    __asm("SET [0xF000], B");
    u16 argument = *p;

    u16 interruptClass = interruptMessage >> 8;
    u16 interruptFunction = interruptMessage & 0xFF;

    switch (interruptClass) {
        case 0x00:
            switch (interruptFunction) {
                case 0x00:
                    lem1802_puts((char*) argument);
                    break;
            }
            break;
    }

    __asm("rfi 0");
}
