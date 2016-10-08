#pragma once

#include "types.h"
#include "asm.h"
#include "kernel/interruptHandler/interruptHandler.h"

typedef struct {
	u16 nDevices;
	u16* ids;
	void** data;
} DevicesList;

typedef struct {
	HardwareInfo hardwareInfo;
	void* (*initFunction)(u16 id, u16 int_number, IntHandler* int_handler_location);
	void (*destroyFunction)(void* data);
	u16 (*updateFunction)(void* data, u16 arg1, u16 arg2, u16 arg3);

	// Must be initialized with nDevices = 0
	DevicesList devicesList;
} Driver;

void hardwareLoop(IntHandler *hardware_int_table, Driver** drivers, u16 n_drivers);
