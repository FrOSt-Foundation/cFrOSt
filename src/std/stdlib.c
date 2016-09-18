#include "stdlib.h"
#include "kernel/interruptHandler/interruptHandler.h"

void memcpy(const void *from, void *to, u16 size) {
    for (u16 i = 0; i < size; i++) {
        ((u16*)to)[i] = ((u16*)from)[i];
    }
}

u16* malloc(u16 size) {
    u16* p;
    interrupt(SOFTINT_MALLOC, size, (u16)&p, 0);
    return p;
}

void free(u16* block) {
    interrupt(SOFTINT_FREE, (u16)block, 0, 0);
}
