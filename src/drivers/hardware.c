#include "types.h"
#include "hardware.h"
#include "lem1802/lem1802.h"
#include "asm.h"
#include "kernel/interruptHandler/interruptHandler.h"

u16 monitor = 0xFFFF;
u16 keyboard = 0xFFFF;
u16 clock = 0xFFFF;

void hardwareLoop(IntHandler *hardware_int_table, Driver* drivers[], u16 n_drivers) {
    u16 nb_devices = asm_hwn();
    for(u16 device = 0; device < nb_devices; ++device) {
        HardwareInfo infos = asm_hwq(device);
        hardware_int_table[device] = 0;

		for(u16 i = 0; i < n_drivers; ++i) {
			if(drivers[i]->hardwareInfo.hardware_id_a == infos.hardware_id_a && drivers[i]->hardwareInfo.hardware_id_b == infos.hardware_id_b) {
				if(drivers[i]->devicesList.nDevices == 0) {
					drivers[i]->devicesList.ids = kmalloc(0, 1);
					drivers[i]->devicesList.data = kmalloc(0, 1);
					drivers[i]->devicesList.ids[0] = device;
					drivers[i]->devicesList.data[0] = drivers[i]->initFunction(device, __SOFTINT_NB + device, &hardware_int_table[device]);
				} else {
					u16* oldIds = drivers[i]->devicesList.ids;
					void** oldData = drivers[i]->devicesList.data;
					drivers[i]->devicesList.ids = kmalloc(0, drivers[i]->devicesList.nDevices + 1);
					drivers[i]->devicesList.data = kmalloc(0, drivers[i]->devicesList.nDevices + 1);

					for(u16 j = 0; j < drivers[i]->devicesList.nDevices; ++j) {
						drivers[i]->devicesList.ids[j] = oldIds[j];
						drivers[i]->devicesList.data[j] = oldData[j];
					}
					kfree(oldIds);
					kfree(oldData);

					drivers[i]->devicesList.ids[drivers[i]->devicesList.nDevices] = device;
					drivers[i]->devicesList.data[drivers[i]->devicesList.nDevices] = drivers[i]->initFunction(device,  __SOFTINT_NB + device, &hardware_int_table[device]);
				}
				drivers[i]->devicesList.nDevices++;
			}
		}
    }
}
