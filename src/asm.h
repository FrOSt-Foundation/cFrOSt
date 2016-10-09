#pragma once

#include "types.h"

typedef struct {
    u16 hardware_id_a;
    u16 hardware_id_b;
    u16 hardware_version;
    u16 manufacturer_a;
    u16 manufacturer_b;
} Hardware_info;

void asm_int (u16 message, u16 arg1, u16 arg2, u16 arg3);
u16 asm_iag ();
void asm_ias (u16);
void asm_iaq (u16);
u16 asm_hwn ();
Hardware_info asm_hwq (u16);
void asm_hwi (u16);
void asm_log (u16);
void asm_brk (u16);
void asm_hlt ();
