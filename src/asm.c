#include "asm.h"

void asm_int (u16 message, u16 arg1, u16 arg2, u16 arg3) {
    register u16 reg_b __asm("B") = arg1;
    register u16 reg_c __asm("C") = arg2;
    register u16 reg_x __asm("X") = arg3;
    __asm("int %0"
          :
          : "X"(message),
            "r"(reg_b),
            "r"(reg_c),
            "r"(reg_x)
          : "memory");
}

u16 asm_iag () {
    u16 ia;
    __asm("IAG %0"
          : "=r"(ia));
    return ia;
}

void asm_ias (u16 address) {
    __asm("IAS %0" ::"X"(address));
}

void asm_iaq (u16 val) {
    __asm("IAQ %0" ::"X"(val));
}

Hardware_info asm_hwq (u16 id) {
    register u16 a __asm("A");
    register u16 b __asm("B");
    register u16 c __asm("C");
    register u16 x __asm("X");
    register u16 y __asm("Y");
    __asm volatile("hwq %[device]"
                   : "=r"(a),
                     "=r"(b),
                     "=r"(c),
                     "=r"(x),
                     "=r"(y)
                   : [device] "X"(id));
    return (Hardware_info){
        .hardware_id_a = a,
        .hardware_id_b = b,
        .hardware_version = c,
        .manufacturer_a = x,
        .manufacturer_b = y
    };
}

u16 asm_hwn () {
    u16 nb_devices;
    __asm("hwn %0"
          : "=r"(nb_devices));
    return nb_devices;
}

void asm_log (u16 msg) {
    __asm("log %0" ::"X"(msg));
}

void asm_brk (u16 msg) {
    __asm("brk %0" ::"X"(msg));
}

void asm_hlt () {
    __asm("hlt 0");
}
