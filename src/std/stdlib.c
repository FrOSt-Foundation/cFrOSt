#include "stdlib.h"

void memcpy(const void *from, void *to, u16 size) {
    for (u16 i = 0; i < size; i++) {
        ((u16*)to)[i] = ((u16*)from)[i];
    }
}

u16* malloc(u16 size) {
    u16* p = (u16 *) 0xF000;
    *(p + 1) = (u16) size;

    interrupt(0x0100);

    return (u16*) *p;
}

void free(u16* block) {
    u16* p = (u16 *) 0xF000;
    *(p + 1) = (u16) block;

    interrupt(0x0101);
}

void mclear(u16* block) {
    u16* p = (u16 *) 0xF000;
    *(p + 1) = (u16) block;

    interrupt(0x0102);
}

u16 msize(u16* block) {
    u16* p = (u16 *) 0xF000;
    *(p + 1) = (u16) block;

    interrupt(0x0103);

    return *p;
}

u16* realloc(u16* block, u16 newSize) {
    if(block < MEMORY_START || block > MEMORY_END)
        return (u16 *) 0xFFFF;

    u16 oldSize = msize(block);
    u16* newBlock = malloc(newSize);
    memcpy(block, newBlock, oldSize);

    free(block);

    return newBlock;
}
