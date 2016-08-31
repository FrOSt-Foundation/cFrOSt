#include "keyboard.h"

#include "kernel/memoryManager/memoryManager.h"

typedef enum {
    CLEAR_BUFFER = 0,
    GET_NEXT_KEY = 1,
    KEY_IS_PRESSED = 2,
    SET_INT_MSG = 3,
} message;

void* keyboard_init(u16 keyboard) {
	Keyboard_driverData *data = kmalloc(0, sizeof(Keyboard_driverData));
	data->keyboard = keyboard;
	data->n_buffer = 0;
	data->buffer = kmalloc(0, 64);

	asm_log(data->keyboard);

	return data;
}

void keyboard_destroy(void* data) {
	kfree(((Keyboard_driverData*) data)->buffer);
}

u16 keyboard_update_function(void* data, u16 message, u16 arg1, u16 arg2) {
	//switch(())
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
    __asm("hwi %0"
          :
          : "X"(data->keyboard),
            "r"(action));
	__asm volatile("" : "=r" (key)); // As it was previously ("=r" (key) inside the hwi block), clang would fuck up and call HWI C instead of HWI B
    return key;
}

bool keyboard_is_pressed(Keyboard_driverData* data, char key) {
    register u16 action __asm("A") = KEY_IS_PRESSED;
    register char arg_b __asm("B") = key;
    register u16 arg_c __asm("C");
    __asm("hwi %0"
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

	char c = '\0';

	do {
		c = keyboard_get_next(data);
	} while(c == '\0');

	do {
		data->buffer[data->n_buffer++] = c;
		c = keyboard_get_next(data);
	} while(c != '\0');

	return data->buffer[--data->n_buffer];
}
