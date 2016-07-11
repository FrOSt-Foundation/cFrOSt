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
static u16 vram[lem1802_vram_size] = {0};
static u16 cursorPos = 0;

IntHandler lem1802_init(u16 monitor, u16 msg __attribute__((unused))) {
    lem1802_set_vram(monitor, vram);
    return NULL;
}

void lem1802_puts(char* s) {
    u16 offset = 0;
    do {
        vram[cursorPos] = 0xF000 | s[offset];
        ++offset;
        ++cursorPos;
    } while(s[offset] != 0);
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
