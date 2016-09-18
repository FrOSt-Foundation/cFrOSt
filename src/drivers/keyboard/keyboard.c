#include "keyboard.h"

#include "kernel/memoryManager/memoryManager.h"

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
; // Fixes color syntax in atom-editor, no real use. Sorry :(

typedef enum {
    CLEAR_BUFFER = 0,
    GET_NEXT_KEY = 1,
    KEY_IS_PRESSED = 2,
    SET_INT_MSG = 3,
} keyboard_action;

void* keyboard_init(u16 keyboard, u16 UNUSED(int_number), IntHandler* UNUSED(int_handler_location)) {
	Keyboard_driverData *data = kmalloc(0, sizeof(Keyboard_driverData));
	data->keyboard = keyboard;
	data->n_buffer = 0;
	data->buffer = kmalloc(0, 64);

	return data;
}

void keyboard_destroy(void* data) {
	kfree(((Keyboard_driverData*) data)->buffer);
}

u16 keyboard_update_function(void* data, u16 message, u16 arg1, u16 UNUSED(arg2)) {
	switch((Keyboard_message) message) {
		case KEYBOARD_CLEAR_BUFFER:
			keyboard_clear_buffer(data);
			break;
		case KEYBOARD_GET_NEXT:
			keyboard_get_next(data);
			break;
		case KEYBOARD_IS_PRESSED:
			return keyboard_is_pressed(data, arg1);
		case KEYBOARD_SET_INT_MSG:
			keyboard_set_int_msg(data, arg1);
			break;
	}
	return 0;
}

void keyboard_clear_buffer(Keyboard_driverData* data) {
    register u16 action __asm("A") = CLEAR_BUFFER;
    __asm("hwi %0"
          :
          : "X"(data->keyboard),
            "r"(action));
}

char keyboard_get_next(Keyboard_driverData* data) {
    register u16 action __asm("A") = GET_NEXT_KEY;
    register char key __asm("C");
    __asm("hwi %1"
          : "=r" (key)
          : "X"(data->keyboard),
            "r"(action));
    return key;
}

bool keyboard_is_pressed(Keyboard_driverData* data, char key) {
    register u16 action __asm("A") = KEY_IS_PRESSED;
    register char arg_b __asm("B") = key;
    register u16 arg_c __asm("C");
    __asm("hwi %1"
          : "=r"(arg_c)
          : "X"(data->keyboard),
            "r"(action),
            "r"(arg_b));
    return arg_c;
}

void keyboard_set_int_msg(Keyboard_driverData* data, u16 msg) {
    register u16 action __asm("A") = SET_INT_MSG;
    register u16 arg_b __asm("B") = msg;
    __asm("hwi %0"
          :
          : "X"(data->keyboard),
            "r"(action),
            "r"(arg_b));
}

char keyboard_getc(Keyboard_driverData* data) {
	if(data->n_buffer != 0) {
		return data->buffer[--data->n_buffer];
	}

	char c = keyboard_get_next(data);

    if(c == '\0')
        return '\0';

	while(c != '\0') {
		data->buffer[data->n_buffer++] = c;
		c = keyboard_get_next(data);
	}

	return data->buffer[--data->n_buffer];
}
