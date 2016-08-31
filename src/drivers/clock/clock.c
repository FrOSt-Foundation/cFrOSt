#include "clock.h"

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
; // Fixes color syntax in atom-editor, no real use. Sorry :(

typedef enum {
    CHANGE_TICKRATE = 0,
    GET_TICKS_SINCE_LAST_CALL = 1,
    CLOCK_SET_INT_MSG = 2,
} command;

void* clock_init(u16 clock) {
	Clock_driverData *data = kmalloc(0, sizeof(Clock_driverData));
	data->clock = clock;

	return data;
}

void clock_destroy(void* UNUSED(data)) {

}

void clock_update_function(void* data, u16 message, u16 arg1, u16 arg2) {
	//TODO
}

void clock_set_tickrate(Clock_driverData* data, u16 rate) {
    register u16 action __asm("A") = CHANGE_TICKRATE;
    register u16 reg_b __asm("B") = rate;
    __asm("hwi %0"
          :
          : "X"(data->clock),
            "r"(action),
            "r"(reg_b));
}

u16 clock_ticks_since_last_call(Clock_driverData* data) {
    register u16 action __asm("A") = GET_TICKS_SINCE_LAST_CALL;
    register u16 reg_c __asm("C");
    __asm("hwi %0"
          :
          : "X"(data->clock),
            "r"(action));
	__asm volatile("" : "=r" (reg_c));
    return reg_c;
}

void clock_set_int_msg(Clock_driverData* data, u16 msg) {
    register u16 action __asm("A") = CLOCK_SET_INT_MSG;
    register u16 reg_b __asm("B") = msg;
    __asm("hwi %0"
          :
          : "X"(data->clock),
            "r"(action),
            "r"(reg_b));
}
