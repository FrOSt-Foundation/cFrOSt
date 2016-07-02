#include "../include/types.h"
#include "../include/defines.h"
#include "hardware.h"

u16 monitor = 0xFFFF;
u16 keyboard = 0xFFFF;
u16 clock = 0xFFFF;

void hardwareLoop() {
    u16 n_devices;
    __asm volatile("hwn %0" : "=m" (n_devices));

    for(u16 device = 0; device < n_devices; ++device) {
        u16 idA = 0, idB = 0;
        __asm volatile("set PUSH, A \n\t\
                        set PUSH, B \n\t\
                        hwq %0 \n\t\
                        set %1, A \n\t\
                        set %2, B \n\t\
                        set B, POP \n\t\
                        set A, POP"
                    : "=m" (device) : "m" (idA), "m" (idB));
        if(idB == 0x7349 && idA == 0xf615) { // Monitor
            monitor = device;
            lem1802_init(monitor);
        } else if(idB == 0x30cf && idA == 0x7406) { // Keyboard
            keyboard = device;
        } else if(idB == 0x12d0 && idA == 0xb402) { // Clock
            clock = device;
        }
    }
}
