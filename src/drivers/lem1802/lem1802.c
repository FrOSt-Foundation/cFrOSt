#include "lem1802.h"

u16 *vram = (u16*)0x8000;
u16 cursorPos = 0;

void lem1802_init(u16 monitor) {
    u16 *p = (u16 *) 0xF000;
    *p = monitor;
    __asm volatile("set PUSH, A\n\t\
                    set PUSH, B\n\t\
                    set A, 0\n\t\
                    set B, 0x8000\n\t\
                    hwi [0xF000]\n\t\
                    set B, POP\n\t\
                    set A, POP");
}

void lem1802_puts(char* s) {
    u16 offset = 0;
    u16* p = vram;
    do {
        *(p + cursorPos) = 0xF000 | *(s + offset);
        ++offset;
        ++cursorPos;
    } while(*(s + offset) != 0);
}
