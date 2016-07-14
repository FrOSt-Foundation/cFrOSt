#include "types.h"
#include "drivers/hardware.h"
#include "mm.h"
#include "interruptHandler/interruptHandler.h"
#include "asm.h"

#include "std/stdio.h"
#include "std/string.h"
#include "std/stdlib.h"

int main(void) {
    mm_init();
    IntHandler *hardware_int_table = int_handler_allocate(asm_hwn());
    hardwareLoop(hardware_int_table);
    int_handler_activate();

    char* loadedMessage = "cFrOSt loaded from disk, and interrupts enabled. Loading modules...";
    puts(loadedMessage);

    char* buffer = (char*) malloc(6);
    uitoa(12345, buffer);
    puts(buffer);
    free((u16*) buffer);

    while(1) {

    }
}
