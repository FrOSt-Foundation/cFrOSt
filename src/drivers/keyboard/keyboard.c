#include "keyboard.h"

#include "kernel/memoryManager/memoryManager.h"

typedef enum {
    CLEAR_BUFFER = 0,
    GET_NEXT_KEY = 1,
    KEY_IS_PRESSED = 2,
    SET_INT_MSG = 3,
} message;

static u16 keyboard_id = 0xFFFF;

static char* keyboard_buffer = NULL;
static u16 keyboard_n_buffer = 0;

void keyboard_init(u16 id) {
	keyboard_id = id;

	if(keyboard_buffer != NULL) {
		kfree(keyboard_buffer);
	}

	keyboard_buffer = kmalloc(0, 16);
	keyboard_n_buffer = 0;
}

u16 keyboard_update_function(u16 message, u16 device, u16 arg1, u16 arg2) {
	//switch(())
}

void keyboard_clear_buffer() {
    register u16 action __asm("A") = CLEAR_BUFFER;
    __asm("hwi %0"
          :
          : "X"(keyboard_id),
            "r"(action));
}

char keyboard_get_next() {
    register u16 action __asm("A") = GET_NEXT_KEY;
    register char key __asm("C");
    __asm("hwi %0"
          :
          : "X"(keyboard_id),
            "r"(action));
	__asm volatile("" : "=r" (key)); // As it was previously ("=r" (key) inside the hwi block), clang would fuck up and call HWI C instead of HWI B
    return key;
}

bool keyboard_is_pressed(char key) {
    register u16 action __asm("A") = KEY_IS_PRESSED;
    register char arg_b __asm("B") = key;
    register u16 arg_c __asm("C");
    __asm("hwi %0"
          : "=r"(arg_c)
          : "X"(keyboard_id),
            "r"(action),
            "r"(arg_b));
    return arg_c;
}

void keyboard_set_int_msg(u16 msg) {
    register u16 action __asm("A") = SET_INT_MSG;
    register u16 arg_b __asm("B") = msg;
    __asm("hwi %0"
          :
          : "X"(keyboard_id),
            "r"(action),
            "r"(arg_b));
}

void keyboard_disable_int() {
    keyboard_set_int_msg(0);
}


char keyboard_getc() {
	if(keyboard_n_buffer != 0) {
		return keyboard_buffer[--keyboard_n_buffer];
	}

	char c = '\0';

	do {
		c = keyboard_get_next();
	} while(c == '\0');

	do {
		keyboard_buffer[keyboard_n_buffer++] = c;
		c = keyboard_get_next();
	} while(c != '\0');

	return keyboard_buffer[--keyboard_n_buffer];
}
