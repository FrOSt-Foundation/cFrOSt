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

typedef struct {
	u16 monitor;
	u16* vram;
} Lem1802_driverData;


void* lem1802_init(u16 monitor, u16 int_number, IntHandler* int_handler_location);
void lem1802_destroy(void* data);
u16 lem1802_update_function(void* data, u16 arg1, u16 arg2, u16 arg3);

#define LEM1802_VRAM_SIZE 386
#define LEM1802_FONT_SIZE 256
#define LEM1802_PALETTE_SIZE 16

void lem1802_loadBuffer(Lem1802_driverData* data, char* buffer);
void lem1802_putc(Lem1802_driverData* data, char c, u16 offset);
void lem1802_scroll(Lem1802_driverData* data);
void lem1802_clear(Lem1802_driverData* data);

void lem1802_set_vram(Lem1802_driverData* data, u16 vram[LEM1802_VRAM_SIZE]);
void lem1802_set_font(Lem1802_driverData* data, u16 font[LEM1802_FONT_SIZE]);
void lem1802_set_palette(Lem1802_driverData* data, u16 palette[LEM1802_PALETTE_SIZE]);
void lem1802_set_border_color(Lem1802_driverData* data, u16 color);
void lem1802_dump_font(Lem1802_driverData* data, u16 font[LEM1802_FONT_SIZE]);
void lem1802_dump_palette(Lem1802_driverData* data, u16 palette[LEM1802_PALETTE_SIZE]);
