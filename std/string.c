#include "string.h"

void memcpy(const void *from, void *to, u16 size) {
    for (u16 i = 0; i < size; i++) {
        ((u16*)to)[i] = ((u16*)from)[i];
    }
}

u16 strlen(const char* s) {
    float len = 0;
    while (*(s + (int)len) != 0) {
        len += 1;
    }
    return len;
}
