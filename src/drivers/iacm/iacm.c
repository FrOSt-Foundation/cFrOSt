#include "iacm.h"

typedef enum {
    ACTION_SET_MODE,
    ACTION_SET_RUN_TIME,
    ACTION_SET_SLEEP_TIME,
    ACTION_SET_INTERRUPT_MESSAGE,
    ACTION_GET_CLOCK_RATE,
    ACTION_GET_RCLOCK_RATE,
    ACTION_RESET_SYSTEM = 0x505
} iacm_action;

Driver driver_iacm = (Driver){
    .hardware_info = (Hardware_info){.hardware_id_a = 0xdacc,
                                     .hardware_id_b = 0x11e0,
                                     .hardware_version = 0x0004,
                                     .manufacturer_a = 0x0000, // Vendor code is defined as "(Various)"
                                     .manufacturer_b = 0x0000 },
    .update_function = iacm_update_function,
    .init_function = iacm_init,
    .destroy_function = iacm_destroy,
    .devices_list = (Devices_list){.n_devices = 0 }
};

void *iacm_init (u16 iacm, u16 UNUSED (int_number), Int_handler *UNUSED (int_handler_location)) {
    Iacm_driver_data *data = kmalloc (0, sizeof (Iacm_driver_data));
    data->iacm = iacm;

    return data;
}


void iacm_destroy (void *UNUSED (data)) {
    // Nothing to free
}

u16 iacm_update_function (void *data, u16 message, u16 arg1, u16 arg2) {
    switch ((Iacm_message)message) {
        case IACM_SET_MODE:
            iacm_set_mode (data, arg1);
            break;
        case IACM_SET_RUN_TIME:
            iacm_set_run_time (data, arg1);
            break;
        case IACM_SET_SLEEP_TIME:
            iacm_set_sleep_time (data, arg1);
            break;
        case IACM_SET_INTERRUPT_MESSAGE:
            iacm_set_interrupt_message (data, arg1);
            break;
        case IACM_GET_CLOCK_RATE:
            iacm_get_clock_rate (data, (u16 *)arg1, (u16 *)arg2);
            break;
        case IACM_GET_RCLOCK_RATE:
            iacm_get_rclock_rate (data, (u16 *)arg1, (u16 *)arg2);
            break;
        case IACM_RESET_SYSTEM:
            iacm_reset_system (data);
            break;
    }

    return 0;
}


void iacm_set_mode (Iacm_driver_data *data, u16 mode) {
    register u16 action __asm("A") = ACTION_SET_MODE;
    register u16 arg_b __asm("B") = mode;
    __asm volatile("hwi %0"
                   :
                   : "X"(data->iacm),
                     "r"(action),
                     "r"(arg_b));
}


void iacm_set_run_time (Iacm_driver_data *data, u16 time) {
    register u16 action __asm("A") = ACTION_SET_RUN_TIME;
    register u16 arg_b __asm("B") = time;
    __asm volatile("hwi %0"
                   :
                   : "X"(data->iacm),
                     "r"(action),
                     "r"(arg_b));
}


void iacm_set_sleep_time (Iacm_driver_data *data, u16 time) {
    register u16 action __asm("A") = ACTION_SET_SLEEP_TIME;
    register u16 arg_b __asm("B") = time;
    __asm volatile("hwi %0"
                   :
                   : "X"(data->iacm),
                     "r"(action),
                     "r"(arg_b));
}


void iacm_set_interrupt_message (Iacm_driver_data *data, u16 message) {
    register u16 action __asm("A") = ACTION_SET_INTERRUPT_MESSAGE;
    register u16 arg_b __asm("B") = message;
    __asm volatile("hwi %0"
                   :
                   : "X"(data->iacm),
                     "r"(action),
                     "r"(arg_b));
}


void iacm_get_clock_rate (Iacm_driver_data *data, u16 *rate_high, u16 *rate_low) {
    register u16 action __asm("A") = ACTION_GET_CLOCK_RATE;
    __asm volatile("hwi %2"
                   : "=X"(*rate_high),
                     "=X"(*rate_low)
                   : "X"(data->iacm),
                     "r"(action));
}


void iacm_get_rclock_rate (Iacm_driver_data *data, u16 *rate_high, u16 *rate_low) {
    register u16 action __asm("A") = ACTION_GET_RCLOCK_RATE;
    __asm volatile("hwi %2"
                   : "=X"(*rate_high),
                     "=X"(*rate_low)
                   : "X"(data->iacm),
                     "r"(action));
}


void iacm_reset_system (Iacm_driver_data *data) {
    register u16 action __asm("A") = ACTION_RESET_SYSTEM;
    __asm volatile("hwi %0"
                   :
                   : "X"(data->iacm),
                     "r"(action));
}
