#include "lem1802.h"

u16 vram = 0x8000;
u16 cursorPos = 0;

void lem1802_init(u16 monitor) {
    __asm volatile("set PUSH, A\n\t\
                    set PUSH, B\n\t\
                    set A, 0\n\t\
                    set B, 0x8000\n\t\
                    hwi %0\n\t\
                    set B, POP\n\t\
                    set A, POP" :: "m" (monitor));
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
