#include "lem1802.h"

typedef enum {
   MEM_MAP_SCREEN = 0,
} lem1802_action ;

u16 *vram = (u16*)0x8000;
u16 cursorPos = 0;

void lem1802_init(u16 monitor) {
    register u16 action __asm("A") = MEM_MAP_SCREEN;
    register u16 *vram_reg __asm("B") = vram;
    __asm("hwi %0"
          :
          : "X"(monitor),
            "r"(action),
            "r"(vram_reg));
}

void lem1802_puts(char* s) {
    u16 offset = 0;
    do {
        vram[cursorPos] = 0xF000 | s[offset];
        ++offset;
        ++cursorPos;
    } while(s[offset] != 0);
}
