#include "asm.h"

void asm_int(u16 message, u16 arg1, u16 arg2) {
    register u16 reg_b __asm ("B") = arg1;
    register u16 reg_c __asm ("C") = arg2;
    __asm ("int %0"
           :
           :"X"(message),
            "r"(reg_b),
            "r"(reg_c)
           : "memory");
}

u16 asm_iag() {
    u16 ia;
    __asm ("IAG %0" : "=r"(ia));
    return ia;
}

void asm_ias(u16 address) {
    __asm ("IAS %0" :: "X"(address));
}

void asm_iaq(u16 val) {
    __asm ("IAQ %0" :: "X"(val));
}

hardware_infos asm_hwq(u16 id) {
    register u16 a __asm ("A");
    register u16 b __asm ("B");
    register u16 c __asm ("C");
    register u16 x __asm ("X");
    register u16 y __asm ("Y");
    __asm volatile("hwq %[device]"
                : "=r" (a),
                  "=r" (b),
                  "=r" (c),
                  "=r" (x),
                  "=r" (y)
                : [device] "X" (id));
    return (hardware_infos) {
        .hardware_id = (u32)a + ((u32)b << 16),
        .hardware_version = c,
        .manufacturer = (u32)x + ((u32)y << 16)
    };
}

u16 asm_hwn() {
    u16 nb_devices;
    __asm("hwn %0" : "=r" (nb_devices));
    return nb_devices;
}

void asm_log(u16 msg) {
    __asm("log %0" :: "X"(msg));
}

void asm_brk(u16 msg) {
    __asm("brk %0" :: "X"(msg));
}

void asm_hlt() {
    __asm("hlt 0");
}
