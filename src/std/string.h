#pragma once

#include "../include/types.h"

void memcpy(const void *from, void *to, u16 size);
u16 strlen(const char* s);
void itoa(i16 n, char* s);
void uitoa(u16 n, char* s);
void reverse(char* s);
