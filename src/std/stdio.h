#pragma once

#include "types.h"

//OUTPUT

void printf (char *s);
void printc (char c);
void scroll (u16 lines);
void newline ();
void move_cursor (u16 x, u16 y);
void clear ();

//INPUT

char getc ();
