void interruptHandler_init() {
    __asm volatile("IAS ____ptr_interruptHandler");

    __asm volatile("SET PC, interruptHandler_init___end \n\t\
                    :____ptr_interruptHandler \n\t\
                    set PUSH, ____ptr_interruptHandler_quit \n\t\
                    set PC, interruptHandler \n\t\
                    :____ptr_interruptHandler_quit \n\t\
                    RFI 0 \n\t\
                    :interruptHandler_init___end");
}

#include "../../std/string.h"

void interrupt(u16 message) {
    u16* p = (u16*) 0xF000;
    *p = message;

    __asm volatile("int [0xF000]");
}

/*
 *  The most significant eight bits of the interrupt message define its class. The following eight bits define the interrupt function within that class.
 *
 *  Interrupt (class then function) :
 *    * 0x00: Fallback I/O. Initialized right after FrOSt and not dependant on module loading, allows the kernel to output messages at any time.
 *      - 0x00: puts
 *    * 0x01: Memory (malloc and such).
 *      - 0x00: malloc
 *      - 0x01: free
 *      - 0x02: clearBlock
 *      - 0x03: size
 *
 *  Arguments are stored sequentially in [0xF001], [0xF002]...
 *  Values are stored sequentially in [0xF000], [0xF001]...
 */

void interruptHandler() {
    u16* p = (u16*) 0xF000;
    u16 interruptMessage = *p;

    u16 interruptClass = interruptMessage >> 8;
    u16 interruptFunction = interruptMessage & 0xFF;

    switch (interruptClass) {
        case 0x00:
            switch (interruptFunction) {
                case 0x00:
                    lem1802_puts((char*) *(p + 1));
                    break;
            }
            break;
        case 0x01:
            switch(interruptFunction) {
                case 0x00:
                    *p = (u16) memoryManager_malloc((u16) *(p + 1));
                    break;
                case 0x01:
                    memoryManager_free((u16 *) *(p + 1));
                    break;
                case 0x02:
                    memoryManager_clear((u16 *) *(p + 1));
                    break;
                case 0x03:
                    *p = memoryManager_size((u16 *) *(p + 1));
                    break;
            }
            break;
    }
}
