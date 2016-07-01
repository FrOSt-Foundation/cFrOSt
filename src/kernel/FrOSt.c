#include "../include/types.h"
#include "../include/defines.h"
#include "../drivers/hardware.h"
#include "../drivers/lem1802/lem1802.h"
#include "interruptHandler/interruptHandler.h"

int main(void) {
    hardwareLoop();
    interruptHandler_init();

    char* loadedMessage = "cFrOSt loaded from disk, and interrupts enabled. Loading modules...";
    interrupt(0, (u16) loadedMessage);

    while(1) {

    }
}
