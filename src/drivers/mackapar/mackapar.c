#include "mackapar.h"

typedef enum {
    ACTION_POLL_DEVICE,
    ACTION_SET_INTERRUPT,
    ACTION_READ_SECTOR,
    ACTION_WRITE_SECTOR,
    ACTION_SPIN_DOWN,
    ACTION_SPIN_UP
} Mackapar_action;

void *mackapar_init_m35fd (u16 mackapar, u16 UNUSED(int_number), Int_handler *UNUSED(int_handler_location)) {
    return mackapar_init(mackapar, MACKAPAR_M35FD);
}

void *mackapar_init_m525hd (u16 mackapar, u16 UNUSED(int_number), Int_handler *UNUSED(int_handler_location)) {
    return mackapar_init(mackapar, MACKAPAR_M525HD);
}

void *mackapar_init (u16 mackapar, Mackapar_type type) {
    Mackapar_driver_data *data = kmalloc (0, sizeof (Mackapar_driver_data));
    data->mackapar = mackapar;
    data->type = type;

    mackapar_spin_up(data); // No need to check the type, at worst this function doesn't do anything

    return data;
}

void mackapar_destroy (void* UNUSED(data)) {

}

u16 mackapar_update_function (void *data, u16 message, u16 arg1, u16 arg2) {
    switch((Mackapar_message) message) {
        case MACKAPAR_POLL_DEVICE:
            mackapar_poll_device(data, (u16*) arg1, (u16*) arg2);
            break;
        case MACKAPAR_SET_INTERRUPT:
            mackapar_set_interrupt(data, arg1);
            break;
        case MACKAPAR_READ_SECTOR:
            mackapar_read_sector(data, arg1, (void*) arg2);
            break;
        case MACKAPAR_WRITE_SECTOR:
            mackapar_write_sector(data, arg1, (void*) arg2);
            break;
        case MACKAPAR_SPIN_DOWN:
            mackapar_spin_down(data);
            break;
        case MACKAPAR_SPIN_UP:
            mackapar_spin_up(data);
            break;
    }

    return 0;
}

// TODO

void mackapar_poll_device(Mackapar_driver_data* data, u16* state, u16* error) {
    register u16 action __asm("A") = ACTION_POLL_DEVICE;
    register u16* arg1 __asm("X") = state;
    register u16* arg2 __asm("X") = error;
    __asm volatile("hwi %2"
                   : "=r"(arg1),
                     "=r"(arg2)
                   : "X"(data->mackapar),
                     "r"(action));
}

void mackapar_set_interrupt(Mackapar_driver_data* data, u16 interrupt) {
    register u16 action __asm("A") = ACTION_SET_INTERRUPT;
    register u16 arg1 __asm("X") = interrupt;
    __asm volatile("hwi %0"
                   :
                   : "X"(data->mackapar),
                     "r"(action),
                     "r"(arg1));
}

void mackapar_read_sector(Mackapar_driver_data* data, u16 sector, void* buffer) {
    register u16 action __asm("A") = ACTION_READ_SECTOR;
    register u16 arg1 __asm("X") = sector;
    register void* arg2 __asm("Y") = buffer;
    __asm volatile("hwi %1"
                   : "=r"(arg2)
                   : "X"(data->mackapar),
                     "r"(action),
                     "r"(arg1));
}

void mackapar_write_sector(Mackapar_driver_data* data, u16 sector, void* buffer) {
    register u16 action __asm("A") = ACTION_WRITE_SECTOR;
    register u16 arg1 __asm("X") = sector;
    register void* arg2 __asm("Y") = buffer;
    __asm volatile("hwi %1"
                   : "=r"(arg2)
                   : "X"(data->mackapar),
                     "r"(action),
                     "r"(arg1));
}

void mackapar_spin_down(Mackapar_driver_data* data) {
    if(data->type == MACKAPAR_M525HD) {
        register u16 action __asm("A") = ACTION_SPIN_DOWN;
        __asm volatile("hwi %0"
                       :
                       : "X"(data->mackapar),
                         "r"(action));
    }
}

void mackapar_spin_up(Mackapar_driver_data* data) {
    if(data->type == MACKAPAR_M525HD) {
        register u16 action __asm("A") = ACTION_SPIN_UP;
        __asm volatile("hwi %0"
                       :
                       : "X"(data->mackapar),
                         "r"(action));
    }
}
