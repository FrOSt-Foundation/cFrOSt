#include "lem1802.h"

typedef enum {
   MEM_MAP_SCREEN = 0,
   MEM_MAP_FONT = 1,
   MEM_MAP_PALETTE = 2,
   SET_BORDER_COLOR = 3,
   MEM_DUMP_FONT = 4,
   MEM_DUMP_PALETTE = 5,
} lem1802_action ;

// Init to 1 to force clang not to use lcomm
static u16** vrams;
static u16 cursorPos = 0;

void lem1802_init(u16 monitor) {
	if(driver_lem1802.nDevices == 1) {
		vrams = kmalloc(0, 1);
		vrams[0] = kmalloc(0, 0x180);
	} else {
		u16** vrams_old = vrams;
		vrams = kmalloc(0, driver_lem1802.nDevices);
		for(u16 i = 0; i < driver_lem1802.nDevices - 1; ++i) {
			vrams[i] = vrams_old[i];
		}
		vrams[driver_lem1802.nDevices - 1] = kmalloc(0, 0x180);
		vrams[1][0] = 0xF065; //DEBUG
		asm_log(vrams[1]);
		asm_log(vrams[1][0]);
		kfree(vrams_old);
	}
    lem1802_set_vram(driver_lem1802.devicesList[monitor], vrams[driver_lem1802.nDevices - 1]);
}

u16 lem1802_update_function(u16 message, u16 arg1, u16 arg2) {
	switch((Lem1802_message) message) {
		case LEM1802_LOADBUFFER:
			lem1802_loadBuffer((char *) arg1);
			break;
		case LEM1802_PUTC:
			lem1802_putc((char) arg1, arg2);
			break;
		case LEM1802_SCROLL:
			lem1802_scroll();
			break;
		case LEM1802_CLEAR:
			lem1802_clear();
			break;
		case LEM1802_SET_VRAM:
			lem1802_set_vram(arg1, (u16*) arg2);
			break;
		case LEM1802_SET_FONT:
			lem1802_set_font(arg1, (u16 *) arg2);
			break;
		case LEM1802_SET_PALETTE:
			lem1802_set_palette(arg1, (u16 *) arg2);
			break;
		case LEM1802_SET_BORDER_COLOR:
			lem1802_set_border_color(arg1, arg2);
			break;
		case LEM1802_DUMP_FONT:
			lem1802_dump_font(arg1, (u16 *) arg2);
			break;
		case LEM1802_DUMP_PALETTE:
			lem1802_dump_palette(arg1, (u16 *) arg2);
			break;
	}
	return 0;
}

void lem1802_loadBuffer(char* buffer) {
	for(u16 i = 0; i < 0x180; ++i) {
		vrams[0][i] = 0xF000 | buffer[i];
	}
}

void lem1802_putc(char c, u16 offset) {
	vrams[0][offset] = 0xF000 | c;
}

void lem1802_scroll() {
	for(u16 i = 0; i < 0x160; ++i) {
		vrams[0][i] = vrams[0][i + 0x20];
	}
	for(u16 i = 0x160; i < 0x180; ++i) {
		vrams[0][i] = 0;
	}
}

void lem1802_clear() {
	for(u16 i = 0; i < 0x180; ++i) {
		vrams[0][i] = 0;
	}
}

void lem1802_set_vram(u16 device, u16 vram[lem1802_vram_size]) {
    register u16 action __asm("A") = MEM_MAP_SCREEN;
    register u16 *vram_reg __asm("B") = vram;
    __asm("hwi %0"
          :
          : "X"(device),
            "r"(action),
            "r"(vram_reg));
}

void lem1802_set_font(u16 device, u16 font[lem1802_font_size]) {
    register u16 action __asm("A") = MEM_MAP_FONT;
    register u16 *reg_b __asm("B") = font;
    __asm("hwi %0"
          :
          : "X"(device),
            "r"(action),
            "r"(reg_b));
}

void lem1802_set_palette(u16 device, u16 palette[lem1802_palette_size]) {
    register u16 action __asm("A") = MEM_MAP_PALETTE;
    register u16 *reg_b __asm("B") = palette;
    __asm("hwi %0"
          :
          : "X"(device),
            "r"(action),
            "r"(reg_b));
}

void lem1802_set_border_color(u16 device, u16 color) {
    register u16 action __asm("A") = SET_BORDER_COLOR;
    register u16 reg_b __asm("B") = color;
    __asm("hwi %0"
          :
          : "X"(device),
            "r"(action),
            "r"(reg_b));
}

void lem1802_dump_font(u16 device, u16 font[lem1802_font_size]) {
    register u16 action __asm("A") = MEM_DUMP_FONT;
    register u16 *reg_b __asm("B") = font;
    __asm("hwi %0"
          :
          : "X"(device),
            "r"(action),
            "r"(reg_b)
          : "memory");
}

void lem1802_dump_palette(u16 device, u16 palette[lem1802_palette_size]) {
    register u16 action __asm("A") = MEM_DUMP_PALETTE;
    register u16 *reg_b __asm("B") = palette;
    __asm("hwi %0"
          :
          : "X"(device),
            "r"(action),
            "r"(reg_b)
          : "memory");
}
