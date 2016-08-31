#pragma once

#include "types.h"

// We define Driver before including interruptHandler.h because it includes files that use Driver.
typedef struct {
	HardwareInfo hardwareInfo;
	void (*initFunction)(u16 id);
	u16 (*updateFunction)(u16 msg, u16 arg1, u16 arg2, u16 arg3);

	// This device list is used so that each driver can associate a sequential list of IDs (0, 1, 2, 3...) to their actual hardware IDs. That way, lem1802_set_vram(0, 0x8000) will always refer to the first LEM1802.
	// nDevices must be initialized to 0 when the Driver is declared
	u16 nDevices;
	u16* devicesList;
} Driver;

#include "kernel/interruptHandler/interruptHandler.h"

extern u16 monitor, keyboard, clock;

void hardwareLoop(IntHandler *hardware_int_table, Driver** drivers, u16 n_drivers);
