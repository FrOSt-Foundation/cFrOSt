#include "include/defines.h"
#include "hardware.h"
#include "lem1802/lem1802.h"

u16 monitor = 0xFFFF;
u16 keyboard = 0xFFFF;
u16 clock = 0xFFFF;

void hardwareLoop() {
    __asm volatile("hwn [0xF000]");
    u16* p = (u16*) 0xF000;
    u16 n_devices = *p;

    u16 idA, idB;
    for(int device = 0; device < n_devices; ++device) {
        *p = device;
        __asm volatile("set PUSH, A \n\t\
                        set PUSH, B \n\t\
                        hwq [0xF000] \n\t\
                        set [0xF000], A \n\t\
                        set [0xF001], B \n\t\
                        set B, POP \n\t\
                        set A, POP");
        idA = *p;
        idB = *(p + 1);
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
