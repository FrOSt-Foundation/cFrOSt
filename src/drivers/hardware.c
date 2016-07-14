#include "types.h"
#include "hardware.h"
#include "lem1802/lem1802.h"
#include "asm.h"
#include "kernel/interruptHandler/interruptHandler.h"

u16 monitor = 0xFFFF;
u16 keyboard = 0xFFFF;
u16 clock = 0xFFFF;

void hardwareLoop(IntHandler *hardware_int_table) {
    u16 nb_devices = asm_hwn();
    for(u16 device = 0; device < nb_devices; ++device) {
        hardware_infos infos = asm_hwq(device);
        hardware_int_table[device] = 0;

        switch (infos.hardware_id) {
            case 0x7349f615:
                monitor = device;
                hardware_int_table[device] = lem1802_init(monitor, __SOFTINT_NB + device);
                break;
            case 0x30cf7406:
                keyboard = device;
                break;
            case 0x12d0b402:
                clock = device;
                break;
        }
    }
}
