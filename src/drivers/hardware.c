#include "types.h"
#include "hardware.h"
#include "lem1802/lem1802.h"
#include "asm.h"

u16 monitor = 0xFFFF;
u16 keyboard = 0xFFFF;
u16 clock = 0xFFFF;

void hardwareLoop() {
    u16 nb_devices = asm_hwn();
    for(u16 device = 0; device < nb_devices; ++device) {
        hardware_infos infos = asm_hwq(device);

        switch (infos.hardware_id) {
            case 0x7349f615:
                monitor = device;
                lem1802_init(monitor);
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
