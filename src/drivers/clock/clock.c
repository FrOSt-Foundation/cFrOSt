#include "clock.h"

#include "kernel/memoryManager/memoryManager.h"

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
; // Fixes color syntax in atom-editor, no real use. Sorry :(

typedef enum {
    CHANGE_TICKRATE = 0,
    GET_TICKS_SINCE_LAST_CALL = 1,
    CLOCK_ACTION_SET_INT_MSG = 2, // "_ACTION" is to avoid duplicate with CLOCK_SET_INT_MSG in Clock_message
} Clock_action;

void* clock_init(u16 clock, u16 int_number, IntHandler* int_handler_location) {
	Clock_driverData *data = kmalloc(0, sizeof(Clock_driverData));
	data->clock = clock;

    *int_handler_location = clock_intHandler;

    clock_set_int_msg(data, int_number);

	return data;
}

void clock_destroy(void* UNUSED(data)) {

}

u16 clock_update_function(void* data, u16 message, u16 arg1, u16 UNUSED(arg2)) {
	switch((Clock_message) message) {
		case CLOCK_SET_TICKRATE:
			clock_set_tickrate(data, arg1);
			break;
		case CLOCK_TICKS_SINCE_LAST_CALL:
			return clock_ticks_since_last_call(data);
		case CLOCK_SET_INT_MSG:
			clock_set_int_msg(data, arg1);
			break;
	}
	return 0;
}

void clock_intHandler(u16 message, u16 arg1, u16 arg2, u16 arg3) {
    __asm("JSR scheduler_switch");
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
	__asm("hwi %1"
          : "=r" (reg_c)
          : "X"(data->clock),
            "r"(action));
    return reg_c;
}

void clock_set_int_msg(Clock_driverData* data, u16 msg) {
    register u16 action __asm("A") = CLOCK_ACTION_SET_INT_MSG;
    register u16 reg_b __asm("B") = msg;
    __asm("hwi %0"
          :
          : "X"(data->clock),
            "r"(action),
            "r"(reg_b));
}
