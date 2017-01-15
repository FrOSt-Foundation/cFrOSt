#include "keyboard.h"

#include "drivers/keyboard/keyboard.h"
#include "kernel/memory_manager/memory_manager.h"

typedef enum {
    CLEAR_BUFFER = 0,
    GET_NEXT_KEY = 1,
    KEY_IS_PRESSED = 2,
    SET_INT_MSG = 3,
} keyboard_action;

Driver driver_keyboard = (Driver){
    .hardware_info = (Hardware_info){.hardware_id_a = 0x7406,
                                     .hardware_id_b = 0x30cf,
                                     .hardware_version = 1,
                                     .manufacturer_a = 0x8b36,
                                     .manufacturer_b = 0x1c6c },
    .update_function = keyboard_update_function,
    .init_function = keyboard_init,
    .destroy_function = keyboard_destroy,
    .devices_list = (Devices_list){.n_devices = 0 }
};

void *keyboard_init (u16 keyboard, u16 UNUSED (int_number), Int_handler *UNUSED (int_handler_location)) {
    Keyboard_driver_data *data = kmalloc (0, sizeof (Keyboard_driver_data));
    data->keyboard = keyboard;

    return data;
}

void keyboard_destroy (void *UNUSED (data)) {
}

u16 keyboard_update_function (void *data, u16 message, u16 arg1, u16 UNUSED (arg2)) {
    switch ((Keyboard_message)message) {
        case KEYBOARD_CLEAR_BUFFER:
            keyboard_clear_buffer (data);
            break;
        case KEYBOARD_GET_NEXT:
            keyboard_get_next (data);
            break;
        case KEYBOARD_IS_PRESSED:
            return keyboard_is_pressed (data, (char)arg1);
        case KEYBOARD_SET_INT_MSG:
            keyboard_set_int_msg (data, arg1);
            break;
    }
    return 0;
}

void keyboard_clear_buffer (Keyboard_driver_data *data) {
    register u16 action __asm("A") = CLEAR_BUFFER;
    __asm("hwi %0"
          :
          : "X"(data->keyboard),
            "r"(action));
}

char keyboard_get_next (Keyboard_driver_data *data) {
    register u16 action __asm("A") = GET_NEXT_KEY;
    register char key __asm("C");
    __asm("hwi %1"
          : "=r"(key)
          : "X"(data->keyboard),
            "r"(action));
    return key;
}

bool keyboard_is_pressed (Keyboard_driver_data *data, char key) {
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

void keyboard_set_int_msg (Keyboard_driver_data *data, u16 msg) {
    register u16 action __asm("A") = SET_INT_MSG;
    register u16 arg_b __asm("B") = msg;
    __asm("hwi %0"
          :
          : "X"(data->keyboard),
            "r"(action),
            "r"(arg_b));
}
