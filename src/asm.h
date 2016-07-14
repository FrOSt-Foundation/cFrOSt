#pragma once

#include "types.h"

typedef struct {
    u32 hardware_id;
    u16 hardware_version;
    u32 manufacturer;
} hardware_infos;

void asm_int(u16, u16, u16);
u16 asm_iag();
void asm_ias(u16);
void asm_iaq(u16);
u16 asm_hwn();
hardware_infos asm_hwq(u16);
void asm_hwi(u16);
void asm_log(u16);
void asm_brk(u16);
void asm_hlt();
