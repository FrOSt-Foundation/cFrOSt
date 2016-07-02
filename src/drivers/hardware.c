#include "include/types.h"
#include "include/defines.h"
#include "hardware.h"
#include "lem1802/lem1802.h"

u16 monitor = 0xFFFF;
u16 keyboard = 0xFFFF;
u16 clock = 0xFFFF;

u16 get_number_devices() {
    u16 nb_devices;
    __asm("hwn %0" : "=r" (nb_devices));
    return nb_devices;
}

void hardwareLoop() {
    u16 nb_devices = get_number_devices();
    for(u16 device = 0; device < nb_devices; ++device) {
        register u16 idA __asm ("A");
        register u16 idB __asm ("B");
        __asm volatile("hwq %0"
                    : "=r" (idA),
                      "=r" (idB)
                    : "X" (device)
                    : "C", "X", "Y");
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
