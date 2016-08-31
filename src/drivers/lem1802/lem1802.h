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

void lem1802_init(u16 monitor);
u16 lem1802_update_function(u16 message, u16 device, u16 arg1, u16 arg2);

const u16 lem1802_vram_size = 386;
const u16 lem1802_font_size = 256;
const u16 lem1802_palette_size = 16;

void lem1802_loadBuffer(u16 device, char* buffer);
void lem1802_putc(u16 device, char c, u16 offset);
void lem1802_scroll(u16 device);
void lem1802_clear(u16 device);

void lem1802_set_vram(u16 device, u16 vram[lem1802_vram_size]);
void lem1802_set_font(u16 device, u16 font[lem1802_font_size]);
void lem1802_set_palette(u16 device, u16 palette[lem1802_palette_size]);
void lem1802_set_border_color(u16 device, u16 color);
void lem1802_dump_font(u16 device, u16 font[lem1802_font_size]);
void lem1802_dump_palette(u16 device, u16 palette[lem1802_palette_size]);
