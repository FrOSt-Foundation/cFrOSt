#include "clock.h"

typedef enum {
    CHANGE_TICKRATE = 0,
    GET_TICKS_SINCE_LAST_CALL = 1,
    SET_INT_MSG = 2,
} command;

void clock_set_tickrate(u16 clock, u16 rate) {
    register u16 action __asm("A") = CHANGE_TICKRATE;
    register u16 reg_b __asm("B") = rate;
    __asm("hwi %0"
          :
          : "X"(clock),
            "r"(action),
            "r"(reg_b));
}

u16 clock_ticks_since_last_call(u16 clock) {
    register u16 action __asm("A") = CHANGE_TICKRATE;
    register u16 reg_c __asm("C");
    __asm("hwi %0"
          : "=r"(reg_c)
          : "X"(clock),
            "r"(action));
    return reg_c;
}

void clock_set_int_msg(u16 clock, u16 msg) {
    register u16 action __asm("A") = CHANGE_TICKRATE;
    register u16 reg_b __asm("B") = msg;
    __asm("hwi %0"
          :
          : "X"(clock),
            "r"(action),
            "r"(reg_b));
}

void clock_disable_int(u16 clock) {
    clock_set_int_msg(clock, 0);
}
