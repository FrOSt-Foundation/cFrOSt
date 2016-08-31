#include "types.h"
#include "hardware.h"
#include "lem1802/lem1802.h"
#include "asm.h"
#include "kernel/interruptHandler/interruptHandler.h"

u16 monitor = 0xFFFF;
u16 keyboard = 0xFFFF;
u16 clock = 0xFFFF;

void hardwareLoop(IntHandler *hardware_int_table, Driver** drivers, u16 n_drivers) {
    u16 nb_devices = asm_hwn();
    for(u16 device = 0; device < nb_devices; ++device) {
        HardwareInfo infos = asm_hwq(device);
        hardware_int_table[device] = 0;

		u32 hardware_id = ((u32) infos.hardware_id_b << 16) | (u32) infos.hardware_id_a;
		for(u16 i = 0; i < n_drivers; ++i) {
			if(drivers[i]->hardwareInfo.hardware_id_a == infos.hardware_id_a && drivers[i]->hardwareInfo.hardware_id_b == infos.hardware_id_b) {
				if(drivers[i]->nDevices == 0) {
					drivers[i]->devicesList = kmalloc(0, 1);
					drivers[i]->devicesList[0] = device;
				} else {
					u16* oldList = drivers[i]->devicesList;
					drivers[i]->devicesList = kmalloc(0, drivers[i]->nDevices + 1);
					for(u16 j = 0; j < drivers[i]->nDevices; ++j)
						drivers[i]->devicesList[j] = oldList[j];
					kfree(oldList);
					drivers[i]->devicesList[drivers[i]->nDevices] = device;
				}
				drivers[i]->nDevices++;
				drivers[i]->initFunction(drivers[i]->nDevices - 1);
			}
		}

        switch (hardware_id) {
            case 0x7349f615:
                //monitor = device;
                //hardware_int_table[device] = lem1802_init(monitor, __SOFTINT_NB + device);
                break;
            case 0x30cf7406:
                keyboard = device;
				keyboard_init(keyboard);
                break;
            case 0x12d0b402:
                clock = device;
                break;
        }
    }
}
