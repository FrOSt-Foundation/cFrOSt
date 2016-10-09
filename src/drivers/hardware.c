#include "hardware.h"
#include "asm.h"
#include "kernel/interrupt_handler/interrupt_handler.h"
#include "lem1802/lem1802.h"
#include "types.h"

u16 monitor = 0xFFFF;
u16 keyboard = 0xFFFF;
u16 clock = 0xFFFF;

void hardware_loop (Int_handler *hardware_int_table, Driver *drivers[], u16 n_drivers) {
    u16 nb_devices = asm_hwn ();
    for (u16 device = 0; device < nb_devices; ++device) {
        Hardware_info infos = asm_hwq (device);
        hardware_int_table[device] = 0;

        for (u16 i = 0; i < n_drivers; ++i) {
            if (drivers[i]->hardware_info.hardware_id_a == infos.hardware_id_a && drivers[i]->hardware_info.hardware_id_b == infos.hardware_id_b) {
                if (drivers[i]->devices_list.n_devices == 0) {
                    drivers[i]->devices_list.ids = kmalloc (0, 1);
                    drivers[i]->devices_list.data = kmalloc (0, 1);
                    drivers[i]->devices_list.ids[0] = device;
                    drivers[i]->devices_list.data[0] = drivers[i]->init_function (device, __SOFTINT_NB + device, &hardware_int_table[device]);
                } else {
                    u16 *old_ids = drivers[i]->devices_list.ids;
                    void **old_data = drivers[i]->devices_list.data;
                    drivers[i]->devices_list.ids = kmalloc (0, drivers[i]->devices_list.n_devices + 1);
                    drivers[i]->devices_list.data = kmalloc (0, drivers[i]->devices_list.n_devices + 1);

                    for (u16 j = 0; j < drivers[i]->devices_list.n_devices; ++j) {
                        drivers[i]->devices_list.ids[j] = old_ids[j];
                        drivers[i]->devices_list.data[j] = old_data[j];
                    }
                    kfree (old_ids);
                    kfree (old_data);

                    drivers[i]->devices_list.ids[drivers[i]->devices_list.n_devices] = device;
                    drivers[i]->devices_list.data[drivers[i]->devices_list.n_devices] = drivers[i]->init_function (device, __SOFTINT_NB + device, &hardware_int_table[device]);
                }
                drivers[i]->devices_list.n_devices++;
            }
        }
    }
}
