#include "keyboard.h"

typedef enum {
    CLEAR_BUFFER = 0,
    GET_NEXT_KEY = 1,
    KEY_IS_PRESSED = 2,
    SET_INT_MSG = 3,
} message;

void keyboard_clear_buffer(u16 keyboard) {
    register u16 action __asm("A") = CLEAR_BUFFER;
    __asm("hwi %0"
          :
          : "X"(keyboard),
            "r"(action));
}

char keyboard_get_next(u16 keyboard) {
    register u16 action __asm("A") = GET_NEXT_KEY;
    register char key __asm("C");
    __asm("hwi %0"
          : "=r"(key)
          : "X"(keyboard),
            "r"(action));
    return key;
}

bool keyboard_is_pressed(u16 keyboard, char key) {
    register u16 action __asm("A") = KEY_IS_PRESSED;
    register char arg_b __asm("B") = key;
    register u16 arg_c __asm("C");
    __asm("hwi %0"
          : "=r"(arg_c)
          : "X"(keyboard),
            "r"(action),
            "r"(arg_b));
    return arg_c;
}

void keyboard_set_int_msg(u16 keyboard, u16 msg) {
    register u16 action __asm("A") = SET_INT_MSG;
    register u16 arg_b __asm("B") = msg;
    __asm("hwi %0"
          :
          : "X"(keyboard),
            "r"(action),
            "r"(arg_b));
}

void keyboard_disable_int(u16 keyboard) {
    keyboard_set_int_msg(keyboard, 0);
}
