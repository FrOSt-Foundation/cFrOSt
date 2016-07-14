#include "stdlib.h"
#include "kernel/interruptHandler/interruptHandler.h"

void memcpy(const void *from, void *to, u16 size) {
    for (u16 i = 0; i < size; i++) {
        ((u16*)to)[i] = ((u16*)from)[i];
    }
}

u16* malloc(u16 size) {
    u16* p;
    interrupt2(SOFTINT_MALLOC, size, (u16)&p);
    return p;
}

void free(u16* block) {
    interrupt1(SOFTINT_FREE, (u16)block);
}

void mclear(u16* block) {
    interrupt1(SOFTINT_CLEAR, (u16)block);
}

u16 msize(u16* block) {
    u16 p;
    interrupt2(SOFTINT_SIZE, (u16)block, (u16)&p);
    return p;
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
