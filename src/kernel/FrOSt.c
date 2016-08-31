#include "types.h"
#include "asm.h"
#include "drivers/hardware.h"
#include "kernel/memoryManager/memoryManager.h"
#include "kernel/stdio/stdio.h"
#include "interruptHandler/interruptHandler.h"

#include "std/stdio.h"
#include "std/string.h"
#include "std/stdlib.h"

#include "usr/console/console.h"

#define N_DRIVERS 1
#include "drivers/lem1802/lem1802.h"

Driver driver_lem1802;

int main(void) {
    mm_init();

    IntHandler *hardware_int_table = int_handler_allocate(asm_hwn());

	Driver* drivers[] = {&driver_lem1802};
	hardwareLoop(hardware_int_table, drivers, N_DRIVERS);

	int_handler_activate();

	stdio_init_output(lem1802);
	stdio_init_input(generic_keyboard);

	console_main();

    while(1) {

    }
}

Driver driver_lem1802 = (Driver) {
	.hardwareInfo = (HardwareInfo) {
		.hardware_id_a = 0xf615,
		.hardware_id_b = 0x7349,
		.hardware_version = 0x1802,
		.manufacturer_a = 0x8b36,
		.manufacturer_b = 0x1c6c
	},
	.updateFunction = lem1802_update_function,
	.initFunction = lem1802_init,
	.nDevices = 0
};

/* Driver driver_keyborad = (Driver) {
	.hardwareInfo = (HardwareInfo) {
		.hardware_id_a = 0x7406,
		.hardware_id_b = 0x30cf,
		.hardware_version = 1,
		.manufacturer_a = 0x8b36,
		.manufacturer_b = 0x1c6c
	},
	.updateFunction = keyboard_update_function,
	.initFunction = keyboard_init;
} */
