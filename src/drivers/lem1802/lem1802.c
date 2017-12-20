#include "lem1802.h"

typedef enum {
    MEM_MAP_SCREEN = 0,
    MEM_MAP_FONT = 1,
    MEM_MAP_PALETTE = 2,
    SET_BORDER_COLOR = 3,
    MEM_DUMP_FONT = 4,
    MEM_DUMP_PALETTE = 5,
} lem1802_action;

Driver driver_lem1802 = (Driver){
    .hardware_info = (Hardware_info){ .hardware_id_a = 0xf615,
                                      .hardware_id_b = 0x7349,
                                      .hardware_version = 0x1802,
                                      .manufacturer_a = 0x8b36,
                                      .manufacturer_b = 0x1c6c },
    .update_function = lem1802_update_function,
    .init_function = lem1802_init,
    .destroy_function = lem1802_destroy,
    .devices_list = (Devices_list){ .n_devices = 0 }
};

void *lem1802_init (u16 monitor, u16 UNUSED (int_number), Int_handler *UNUSED (int_handler_location)) {
    Lem1802_driver_data *data = kmalloc (0, sizeof (Lem1802_driver_data));
    data->monitor = monitor;

    lem1802_set_vram (data, kmalloc (0, 0x180));

    return data;
}

void lem1802_destroy (void *data) {
    kfree (((Lem1802_driver_data *)data)->vram);
}

u16 lem1802_update_function (void *data, u16 message, u16 arg1, u16 arg2) {
    switch ((Lem1802_message)message) {
        case LEM1802_LOADBUFFER:
            lem1802_load_buffer (data, (char *)arg1);
            break;
        case LEM1802_PUTC:
            lem1802_putc (data, (char)arg1, arg2);
            break;
        case LEM1802_SCROLL:
            lem1802_scroll (data);
            break;
        case LEM1802_CLEAR:
            lem1802_clear (data);
            break;
        case LEM1802_SET_VRAM:
            lem1802_set_vram (data, (u16 *)arg1);
            break;
        case LEM1802_SET_FONT:
            lem1802_set_font (data, (u16 *)arg1);
            break;
        case LEM1802_SET_PALETTE:
            lem1802_set_palette (data, (u16 *)arg1);
            break;
        case LEM1802_SET_BORDER_COLOR:
            lem1802_set_border_color (data, arg1);
            break;
        case LEM1802_DUMP_FONT:
            lem1802_dump_font (data, (u16 *)arg1);
            break;
        case LEM1802_DUMP_PALETTE:
            lem1802_dump_palette (data, (u16 *)arg1);
            break;
    }
    return 0;
}

void lem1802_load_buffer (Lem1802_driver_data *data, char *buffer) {
    for (u16 i = 0; i < 0x180; ++i) {
        data->vram[i] = 0xF000 | (u16)buffer[i];
    }
}

void lem1802_putc (Lem1802_driver_data *data, char c, u16 offset) {
    data->vram[offset] = 0xF000 | (u16)c;
}

void lem1802_scroll (Lem1802_driver_data *data) {
    for (u16 i = 0; i < 0x160; ++i) {
        data->vram[i] = data->vram[i + 0x20];
    }
    for (u16 i = 0x160; i < 0x180; ++i) {
        data->vram[i] = 0;
    }
}

void lem1802_clear (Lem1802_driver_data *data) {
    for (u16 i = 0; i < 0x180; ++i) {
        data->vram[i] = 0;
    }
}

void lem1802_set_vram (Lem1802_driver_data *data, u16 vram[LEM1802_VRAM_SIZE]) {
    register u16 action __asm("A") = MEM_MAP_SCREEN;
    register u16 *vram_reg __asm("B") = vram;
    __asm("hwi %0"
          :
          : "X"(data->monitor),
            "r"(action),
            "r"(vram_reg));

    data->vram = vram;
}

void lem1802_set_font (Lem1802_driver_data *data, u16 font[LEM1802_FONT_SIZE]) {
    register u16 action __asm("A") = MEM_MAP_FONT;
    register u16 *reg_b __asm("B") = font;
    __asm("hwi %0"
          :
          : "X"(data->monitor),
            "r"(action),
            "r"(reg_b));
}

void lem1802_set_palette (Lem1802_driver_data *data, u16 palette[LEM1802_PALETTE_SIZE]) {
    register u16 action __asm("A") = MEM_MAP_PALETTE;
    register u16 *reg_b __asm("B") = palette;
    __asm("hwi %0"
          :
          : "X"(data->monitor),
            "r"(action),
            "r"(reg_b));
}

void lem1802_set_border_color (Lem1802_driver_data *data, u16 color) {
    register u16 action __asm("A") = SET_BORDER_COLOR;
    register u16 reg_b __asm("B") = color;
    __asm("hwi %0"
          :
          : "X"(data->monitor),
            "r"(action),
            "r"(reg_b));
}

void lem1802_dump_font (Lem1802_driver_data *data, u16 font[LEM1802_FONT_SIZE]) {
    register u16 action __asm("A") = MEM_DUMP_FONT;
    register u16 *reg_b __asm("B") = font;
    __asm("hwi %0"
          :
          : "X"(data->monitor),
            "r"(action),
            "r"(reg_b)
          : "memory");
}

void lem1802_dump_palette (Lem1802_driver_data *data, u16 palette[LEM1802_PALETTE_SIZE]) {
    register u16 action __asm("A") = MEM_DUMP_PALETTE;
    register u16 *reg_b __asm("B") = palette;
    __asm("hwi %0"
          :
          : "X"(data->monitor),
            "r"(action),
            "r"(reg_b)
          : "memory");
}
