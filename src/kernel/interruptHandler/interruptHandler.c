#include "std/string.h"
#include "interruptHandler.h"

static void interruptHandler(u16 msg);

void interruptHandler_init() {
    __asm ("IAS %0" :: "X"(&interruptHandler));
}

void interrupt(u16 message) {
    // An int can modify all the registers
    __asm ("int %0"
           :
           :"X"(message)
           : "A", "B", "C", "X", "Y", "Z", "I", "Y");
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
 *  Arguments are stored sequentially in [0xF000], [0xF001]...
 *  Values are stored sequentially in [0xF000], [0xF001]...
 */
__attribute__ ((interrupt))
static void interruptHandler(u16 msg) {
    // the first argument is passed in A so it should be ok.
    // TODO: make sure it's OK.
    u16* args = (u16*) 0xF000;

    u16 interruptClass = msg >> 8;
    u16 interruptFunction = msg & 0xFF;

    switch (interruptClass) {
        case 0x00:
            switch (interruptFunction) {
                case 0x00:
                    lem1802_puts((char*) args[0]);
                    break;
            }
            break;
        case 0x01:
            switch(interruptFunction) {
                case 0x00:
                    args[0] = (u16) memoryManager_malloc(args[0]);
                    break;
                case 0x01:
                    memoryManager_free((u16 *) args[0]);
                    break;
                case 0x02:
                    memoryManager_clear((u16 *) args[0]);
                    break;
                case 0x03:
                    args[0] = memoryManager_size((u16 *) args[0]);
                    break;
            }
            break;
    }
}
