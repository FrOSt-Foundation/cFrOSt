#include "../include/types.h"
#include "../include/defines.h"
#include "../drivers/hardware.h"
#include "../drivers/lem1802/lem1802.h"

int main(void) {
    hardwareLoop();

    lem1802_puts("Hello, world!");

    while(1) {

    }
}
