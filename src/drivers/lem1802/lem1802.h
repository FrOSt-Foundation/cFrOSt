#pragma once

#include "types.h"
#include "kernel/interruptHandler/interruptHandler.h"

#include "drivers/hardware.h"

typedef enum {
	LEM1802_LOADBUFFER,
	LEM1802_PUTC,
	LEM1802_SCROLL,
	LEM1802_CLEAR,
	LEM1802_SET_VRAM,
	LEM1802_SET_FONT,
	LEM1802_SET_PALETTE,
	LEM1802_SET_BORDER_COLOR,
	LEM1802_DUMP_FONT,
	LEM1802_DUMP_PALETTE
} Lem1802_message;

typedef struct Lem1802_driverData Lem1802_driverData;

void* lem1802_init(u16 monitor, u16 int_number, IntHandler* int_handler_location);
void lem1802_destroy(void* data);
u16 lem1802_update_function(void* data, u16 arg1, u16 arg2, u16 arg3);

const u16 LEM1802_VRAM_SIZE = 386;
const u16 LEM1802_FONT_SIZE = 256;
const u16 LEM1802_PALETTE_SIZE = 16;

void lem1802_loadBuffer(struct Lem1802_driverData* data, char* buffer);
void lem1802_putc(struct Lem1802_driverData* data, char c, u16 offset);
void lem1802_scroll(struct Lem1802_driverData* data);
void lem1802_clear(struct Lem1802_driverData* data);

void lem1802_set_vram(struct Lem1802_driverData* data, u16 vram[LEM1802_VRAM_SIZE]);
void lem1802_set_font(struct Lem1802_driverData* data, u16 font[LEM1802_FONT_SIZE]);
void lem1802_set_palette(struct Lem1802_driverData* data, u16 palette[LEM1802_PALETTE_SIZE]);
void lem1802_set_border_color(struct Lem1802_driverData* data, u16 color);
void lem1802_dump_font(struct Lem1802_driverData* data, u16 font[LEM1802_FONT_SIZE]);
void lem1802_dump_palette(struct Lem1802_driverData* data, u16 palette[LEM1802_PALETTE_SIZE]);
