#include "types.h"
#include "asm.h"
#include "drivers/hardware.h"
#include "kernel/memoryManager/memoryManager.h"
#include "kernel/stdio/stdio.h"
#include "interruptHandler/interruptHandler.h"
#include "kernel/scheduler/scheduler.h"
#include "kernel/panic/panic.h"

#include "std/stdio.h"
#include "std/string.h"
#include "std/stdlib.h"

#include "usr/console/console.h"

#include "drivers/lem1802/lem1802.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/clock/clock.h"

Driver driver_lem1802;
Driver driver_keyboard;
Driver driver_clock;

#define N_DRIVERS 3
static Driver* drivers[] = {&driver_lem1802, &driver_keyboard, &driver_clock};

typedef struct Clock_driverData {
	u16 clock;
} Clock_driverData;

int main(void) {
    mm_init();

    IntHandler *hardware_int_table = int_handler_allocate(asm_hwn());
	hardwareLoop(hardware_int_table, drivers, N_DRIVERS);

	int_handler_activate();

	stdio_init_output(lem1802, &driver_lem1802);
	stdio_init_input(generic_keyboard, &driver_keyboard);

	for(u16 i = 0; i < driver_lem1802.devicesList.nDevices; ++i) {
		stdio_set_current_output(i);
		clear();
	}

	if(driver_clock.devicesList.nDevices == 0 || driver_lem1802.devicesList.nDevices == 0) {
		kpanic("Error: At least one generic clock and one LEM1802 are needed. Please connect it/them to the DCPU and try again.");
	}

	addProcess(&console_main, "console");

	if(driver_lem1802.devicesList.nDevices > 1) {
		for(u16 i = 0; i < driver_lem1802.devicesList.nDevices; ++i) {
			stdio_set_current_output(i);
			printf("Press 1, 2, ... to load console on corresponding screen.\nThis screen is number: ");
			char* buffer = (char *) malloc(6); // 0xFFFF is only 5 digits in base 10
			itoa(i + 1, buffer);
			printf(buffer);
			free((u16 *) buffer);
		}

		char c = '\0';
		do {
			c = getc();
		} while((u16) (c - '0') > driver_lem1802.devicesList.nDevices);

		for(u16 i = 0; i < driver_lem1802.devicesList.nDevices; ++i) {
			if(i != (u16) (c - '0') - 1) {
				stdio_set_current_output(i);
				clear();
			}
		}

		stdio_set_current_output((u16) (c - '0' - 1));
	}

	scheduler_start(&driver_clock);

	if(driver_lem1802.devicesList.nDevices != 0) {
		for(u16 i = 0; i < driver_lem1802.devicesList.nDevices; ++i) {
			stdio_set_current_output(i);
			printf("You can safely shut down the system.");
		}
	}
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
	.destroyFunction = lem1802_destroy,
	.devicesList = (DevicesList) {
		.nDevices = 0
	}
};

typedef struct Lem1802_driverData {
	u16 monitor;
	u16* vram;
} Lem1802_driverData;

Driver driver_keyboard = (Driver) {
	.hardwareInfo = (HardwareInfo) {
		.hardware_id_a = 0x7406,
		.hardware_id_b = 0x30cf,
		.hardware_version = 1,
		.manufacturer_a = 0x8b36,
		.manufacturer_b = 0x1c6c
	},
	.updateFunction = keyboard_update_function,
	.initFunction = keyboard_init,
	.destroyFunction = keyboard_destroy,
	.devicesList = (DevicesList) {
		.nDevices = 0
	}
};

typedef struct Keyboard_driverData {
	u16 keyboard;
	char* buffer;
	u16 n_buffer;
} Keyboard_driverData;

Driver driver_clock = (Driver) {
	.hardwareInfo = (HardwareInfo)  {
		.hardware_id_a = 0xb402,
		.hardware_id_b = 0x12d0,
		.hardware_version = 1,
		.manufacturer_a = 0x8b36,
		.manufacturer_b = 0x1c6c
	},
	.updateFunction = clock_update_function,
	.initFunction = clock_init,
	.destroyFunction = clock_destroy,
	.devicesList = (DevicesList) {
		.nDevices = 0
	}
};
