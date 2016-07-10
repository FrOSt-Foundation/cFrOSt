#pragma once

#include "stdbool.h"
#include "types.h"

#define KEY_BACKSPACE   0x10
#define KEY_RETURN      0x11
#define KEY_INSERT      0x12
#define KEY_DELETE      0x13
#define KEY_ARROW_UP    0x80
#define KEY_ARROW_DOWN  0x81
#define KEY_ARROW_LEFT  0x82
#define KEY_ARROW_RIGHT 0x83
#define KEY_SHIFT       0x90
#define KEY_CONTROL     0x91

void keyboard_clear_buffer(u16 keyboard);
char keyboard_get_next(u16 keyboard);
bool keyboard_is_pressed(u16 keyboard, char key);
void keyboard_set_int_msg(u16 keyboard, u16 msg);
void keyboard_disable_int(u16 keyboard);
