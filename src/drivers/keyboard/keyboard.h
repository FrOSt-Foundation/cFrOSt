#pragma once

#include "kernel/interrupt_handler/interrupt_handler.h"
#include "stdbool.h"
#include "types.h"

typedef enum {
    KEYBOARD_CLEAR_BUFFER,
    KEYBOARD_GET_NEXT,
    KEYBOARD_IS_PRESSED,
    KEYBOARD_SET_INT_MSG
} Keyboard_message;

typedef struct {
    u16 keyboard;
} Keyboard_driver_data;

void *keyboard_init (u16 keyboard, u16 int_number, Int_handler *int_handler_location);
void keyboard_destroy (void *data);
u16 keyboard_update_function (void *data, u16 message, u16 arg1, u16 arg2);

#define KEY_BACKSPACE 0x10
#define KEY_RETURN 0x11
#define KEY_INSERT 0x12
#define KEY_DELETE 0x13
#define KEY_ARROW_UP 0x80
#define KEY_ARROW_DOWN 0x81
#define KEY_ARROW_LEFT 0x82
#define KEY_ARROW_RIGHT 0x83
#define KEY_SHIFT 0x90
#define KEY_CONTROL 0x91

void keyboard_clear_buffer (Keyboard_driver_data *data);
char keyboard_get_next (Keyboard_driver_data *data);
bool keyboard_is_pressed (Keyboard_driver_data *data, char key);
void keyboard_set_int_msg (Keyboard_driver_data *data, u16 msg);

char keyboard_getc (Keyboard_driver_data *data);
