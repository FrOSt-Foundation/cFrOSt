#include "lem1802.h"

u16 vram = 0x8000;
u16 cursorPos = 0;

void lem1802_init(u16 monitor) {
    u16 *p = (u16 *) 0xF000;
    *p = monitor;
    asm("set PUSH, A \n set PUSH, B \n set A, 0 \n set B, 0x8000 \n hwi [0xF000] \n set B, POP \n set A, POP");
}

void lem1802_puts(char* s) {
    u16 offset = 0;
    u16* p = (u16 *) vram;
    do {
        *(p + cursorPos) = 0xF000 | *(s + offset);
        ++offset;
        ++cursorPos;
    } while(*(s + offset) != 0);
}
