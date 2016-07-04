#include "types.h"
#include "drivers/hardware.h"
#include "memoryManager/memoryManager.h"
#include "interruptHandler/interruptHandler.h"

#include "std/stdio.h"
#include "std/string.h"
#include "std/stdlib.h"

int main(void) {
    hardwareLoop();
    memoryManager_init();
    interruptHandler_init();

    char* loadedMessage = "cFrOSt loaded from disk, and interrupts enabled. Loading modules...";
    puts(loadedMessage);

    char* buffer = (char*) malloc(6);
    uitoa(12345, buffer);
    puts(buffer);
    free((u16*) buffer);

    while(1) {

    }
}
