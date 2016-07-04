#include "lem1802.h"

typedef enum {
   MEM_MAP_SCREEN = 0,
} lem1802_action ;

// Init to 1 to force clang not to use lcomm
static u16 vram[386] = {1};
static u16 cursorPos = 1;

void lem1802_init(u16 monitor) {
    cursorPos = 0;
    vram[0] = 0;
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
