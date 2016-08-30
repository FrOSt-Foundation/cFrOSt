#pragma once

#include "types.h"

void memcpy(const void *from, void *to, u16 size);
u16 strlen(const char* s);
u16 strcmp(const char* str1, const char* str2);
void itoa(i16 n, char* s);
void uitoa(u16 n, char* s);
void reverse(char* s);
