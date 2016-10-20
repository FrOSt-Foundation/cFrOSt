#include "clock.h"

#include "drivers/hardware.h"
#include "kernel/memory_manager/memory_manager.h"

typedef enum {
    CHANGE_TICKRATE = 0,
    GET_TICKS_SINCE_LAST_CALL = 1,
    CLOCK_ACTION_SET_INT_MSG = 2, // "_ACTION" is to avoid duplicate with CLOCK_SET_INT_MSG in Clock_message
} Clock_action;

Driver driver_clock = (Driver){
    .hardware_info = (Hardware_info){.hardware_id_a = 0xb402,
                                     .hardware_id_b = 0x12d0,
                                     .hardware_version = 1,
                                     .manufacturer_a = 0x8b36,
                                     .manufacturer_b = 0x1c6c },
    .update_function = clock_update_function,
    .init_function = clock_init,
    .destroy_function = clock_destroy,
    .devices_list = (Devices_list){.n_devices = 0 }
};

void *clock_init (u16 clock, u16 UNUSED (int_number), Int_handler *UNUSED (int_handler_location)) {
    Clock_driver_data *data = kmalloc (0, sizeof (Clock_driver_data));
    data->clock = clock;

    clock_set_int_msg (data, 0xFFFE);

    return data;
}

void clock_destroy (void *UNUSED (data)) {
}

u16 clock_update_function (void *data, u16 message, u16 arg1, u16 UNUSED (arg2)) {
    switch ((Clock_message)message) {
        case CLOCK_SET_TICKRATE:
            clock_set_tickrate (data, arg1);
            break;
        case CLOCK_TICKS_SINCE_LAST_CALL:
            return clock_ticks_since_last_call (data);
        case CLOCK_SET_INT_MSG:
            clock_set_int_msg (data, arg1);
            break;
    }
    return 0;
}

void clock_int_handler (u16 UNUSED (message), u16 UNUSED (arg1), u16 UNUSED (arg2), u16 UNUSED (arg3)) {
    __asm("JSR scheduler_switch");
}

void clock_set_tickrate (Clock_driver_data *data, u16 rate) {
    register u16 action __asm("A") = CHANGE_TICKRATE;
    register u16 reg_b __asm("B") = rate;
    __asm("hwi %0"
          :
          : "X"(data->clock),
            "r"(action),
            "r"(reg_b));
}

u16 clock_ticks_since_last_call (Clock_driver_data *data) {
    register u16 action __asm("A") = GET_TICKS_SINCE_LAST_CALL;
    register u16 reg_c __asm("C");
    __asm("hwi %1"
          : "=r"(reg_c)
          : "X"(data->clock),
            "r"(action));
    return reg_c;
}

void clock_set_int_msg (Clock_driver_data *data, u16 msg) {
    register u16 action __asm("A") = CLOCK_ACTION_SET_INT_MSG;
    register u16 reg_b __asm("B") = msg;
    __asm("hwi %0"
          :
          : "X"(data->clock),
            "r"(action),
            "r"(reg_b));
}
