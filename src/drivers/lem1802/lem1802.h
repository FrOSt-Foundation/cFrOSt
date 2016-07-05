#pragma once

#include "types.h"

const u16 lem1802_vram_size = 386;
const u16 lem1802_font_size = 256;
const u16 lem1802_palette_size = 16;

void lem1802_init(u16 monitor);
void lem1802_puts(char* s);

void lem1802_set_vram(u16 device, u16 vram[lem1802_vram_size]);
void lem1802_set_font(u16 device, u16 font[lem1802_font_size]);
void lem1802_set_palette(u16 device, u16 palette[lem1802_palette_size]);
void lem1802_set_border_color(u16 device, u16 color);
void lem1802_dump_font(u16 device, u16 font[lem1802_font_size]);
void lem1802_dump_palette(u16 device, u16 palette[lem1802_palette_size]);
