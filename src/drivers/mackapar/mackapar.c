#include "mackapar.h"

typedef enum {
    ACTION_POLL_DEVICE,
    ACTION_SET_INTERRUPT,
    ACTION_READ_SECTOR,
    ACTION_WRITE_SECTOR,
    ACTION_SPIN_DOWN,
    ACTION_SPIN_UP
} Mackapar_action;

Driver driver_m35fd = (Driver){
    .hardware_info = (Hardware_info){.hardware_id_a = 0x24c5,
                                     .hardware_id_b = 0x4fd5,
                                     .hardware_version = 0xb,
                                     .manufacturer_a = 0x7e91,
                                     .manufacturer_b = 0x1eb3 },
    .update_function = mackapar_update_function,
    .init_function = mackapar_init_m35fd,
    .destroy_function = mackapar_destroy,
    .devices_list = (Devices_list){.n_devices = 0 }
};

Driver driver_m525hd = (Driver){
    .hardware_info = (Hardware_info){.hardware_id_a = 0x525d,
                                     .hardware_id_b = 0x4ac5,
                                     .hardware_version = 1,
                                     .manufacturer_a = 0x7e91,
                                     .manufacturer_b = 0x1eb3 },
    .update_function = mackapar_update_function,
    .init_function = mackapar_init_m525hd,
    .destroy_function = mackapar_destroy,
    .devices_list = (Devices_list){.n_devices = 0 }
};
void *mackapar_init_m35fd (u16 mackapar, u16 UNUSED (int_number), Int_handler *UNUSED (int_handler_location)) {
    return mackapar_init (mackapar, MACKAPAR_M35FD);
}

void *mackapar_init_m525hd (u16 mackapar, u16 UNUSED (int_number), Int_handler *UNUSED (int_handler_location)) {
    return mackapar_init (mackapar, MACKAPAR_M525HD);
}

void *mackapar_init (u16 mackapar, Mackapar_type type) {
    Mackapar_driver_data *data = kmalloc (0, sizeof (Mackapar_driver_data));
    data->mackapar = mackapar;
    data->type = type;

    mackapar_spin_up (data); // No need to check the type, at worst this function doesn't do anything

    return data;
}

void mackapar_destroy (void *UNUSED (data)) {
}

u16 mackapar_update_function (void *data, u16 message, u16 arg1, u16 arg2) {
    switch ((Mackapar_message)message) {
        case MACKAPAR_POLL_DEVICE:
            mackapar_poll_device (data, (Mackapar_state *)arg1, (Mackapar_error *)arg2);
            break;
        case MACKAPAR_SET_INTERRUPT:
            mackapar_set_interrupt (data, arg1);
            break;
        case MACKAPAR_READ_SECTOR:
            mackapar_read_sector (data, arg1, (void *)arg2);
            break;
        case MACKAPAR_WRITE_SECTOR:
            mackapar_write_sector (data, arg1, (void *)arg2);
            break;
        case MACKAPAR_SPIN_DOWN:
            mackapar_spin_down (data);
            break;
        case MACKAPAR_SPIN_UP:
            mackapar_spin_up (data);
            break;
    }

    return 0;
}

void *mackapar_read (Mackapar_driver_data *data, u32 location, u16 length) {
    Mackapar_state state;
    Mackapar_error error;
    mackapar_poll_device (data, &state, &error);
    if (state == STATE_NO_MEDIA)
        return 0;
    else if (state == STATE_BUSY || state == STATE_INIT || state == STATE_INIT_WP)
        mackapar_wait_until_ready (data);
    else if (state == STATE_PARKED || state == STATE_PARKED_WP) {
        mackapar_spin_up (data);
        mackapar_wait_until_ready (data);
    }

    u16 sector = (u16) (location / WORDS_PER_SECTOR);
    u16 offset = (u16) (location % WORDS_PER_SECTOR);

    u16 n_sectors = (length + offset) / WORDS_PER_SECTOR + ((length + offset) % WORDS_PER_SECTOR == 0 ? 0 : 1);

    u16 *buffer = kmalloc (0, n_sectors * 512);

    for (u16 i = 0; i < n_sectors; ++i) {
        mackapar_read_sector (data, sector + i, buffer + i * WORDS_PER_SECTOR);
        mackapar_wait_until_ready (data);
    }

    if (offset != 0 || length % WORDS_PER_SECTOR != 0) { // If we didn't read a sector (so we have a partial read on either the first or last sector)
        u16 *final_buffer = kmalloc (0, length);

        for (u16 i = 0; i < length; ++i) {
            final_buffer[i] = buffer[i + offset];
        }

        kfree (buffer);

        return final_buffer;
    } else {
        return buffer;
    }
}

bool mackapar_write (Mackapar_driver_data *data, u32 location, u16 length, u16 *d) {
    Mackapar_state state;
    Mackapar_error error;
    mackapar_poll_device (data, &state, &error);
    if (state == STATE_NO_MEDIA || state == STATE_INIT_WP || state == STATE_PARKED_WP || state == STATE_READY_WP)
        return false;
    else if (state == STATE_BUSY || state == STATE_INIT)
        mackapar_wait_until_ready (data);
    else if (state == STATE_PARKED) {
        mackapar_spin_up (data);
        mackapar_wait_until_ready (data);
    }

    u16 sector = (u16) (location / WORDS_PER_SECTOR);
    u16 offset = (u16) (location % WORDS_PER_SECTOR);

    if (offset != 0) {
        u16 *md = (u16 *)kmalloc (0, length + offset + ((length + offset) % WORDS_PER_SECTOR));
        for (u16 i = 0; i < length; ++i) {
            md[i + offset] = *(d + i);
        }

        // There may already have been stuff written on the beggining of the first sector (between location and location + offset), so we save it
        u16 *previousData = mackapar_read (data, sector * WORDS_PER_SECTOR, offset);
        for (u16 i = 0; i < offset; ++i) {
            md[i] = previousData[i];
        }
        kfree (previousData);

        if ((length + offset) % WORDS_PER_SECTOR != 0) { // Like before, if we are writing to a partial sector, we need to save the rest
            previousData = mackapar_read (data, sector * WORDS_PER_SECTOR + length + offset, WORDS_PER_SECTOR - ((length + offset) % WORDS_PER_SECTOR));
            for (u16 i = 0; i < WORDS_PER_SECTOR - ((length + offset) % WORDS_PER_SECTOR); ++i) {
                md[length + offset + i] = previousData[i];
            }
            kfree (previousData);
        }

        d = md;
        length = length + offset + ((length + offset) % WORDS_PER_SECTOR);
    }

    for (u16 i = 0; i < length / WORDS_PER_SECTOR; ++i) {
        mackapar_write_sector (data, sector + i, d + i * WORDS_PER_SECTOR);
        mackapar_wait_until_ready (data);
    }

    if (offset == 0 && length % WORDS_PER_SECTOR != 0) { // If there is an incomplete sector to write
        u16 *msector = (u16 *)kmalloc (0, WORDS_PER_SECTOR);
        for (u16 i = 0; i < length % WORDS_PER_SECTOR; ++i) {
            msector[i] = (u16) * ((u16 *)d + length - (length % WORDS_PER_SECTOR) + i);
        }
        mackapar_write_sector (data, sector + (length / WORDS_PER_SECTOR), msector);
        mackapar_wait_until_ready (data);
        kfree (msector);
    } else if (offset != 0) {
        kfree (d);
    }

    return true;
}

void mackapar_wait_until_ready (Mackapar_driver_data *data) {
    Mackapar_state state;
    Mackapar_error error;
    do {
        mackapar_poll_device (data, &state, &error);
    } while (state != STATE_READY && state != STATE_READY_WP);
}

void mackapar_poll_device (Mackapar_driver_data *data, Mackapar_state *state, Mackapar_error *error) {
    register u16 action __asm("A") = ACTION_POLL_DEVICE;
    register u16 arg1 __asm("B");
    register u16 arg2 __asm("C");
    __asm volatile("hwi %2"
                   : "=r"(arg1),
                     "=r"(arg2)
                   : "X"(data->mackapar),
                     "r"(action));
    *state = (Mackapar_state)arg1;
    *error = (Mackapar_error)arg2;
}

void mackapar_set_interrupt (Mackapar_driver_data *data, u16 interrupt) {
    register u16 action __asm("A") = ACTION_SET_INTERRUPT;
    register u16 arg1 __asm("X") = interrupt;
    __asm volatile("hwi %0"
                   :
                   : "X"(data->mackapar),
                     "r"(action),
                     "r"(arg1));
}

void mackapar_read_sector (Mackapar_driver_data *data, u16 sector, void *buffer) {
    register u16 action __asm("A") = ACTION_READ_SECTOR;
    register u16 arg1 __asm("X") = sector;
    register void *arg2 __asm("Y") = buffer;
    __asm volatile("hwi %0"
                   :
                   : "X"(data->mackapar),
                     "r"(action),
                     "r"(arg1),
                     "r"(arg2));
}

void mackapar_write_sector (Mackapar_driver_data *data, u16 sector, void *buffer) {
    register u16 action __asm("A") = ACTION_WRITE_SECTOR;
    register u16 arg1 __asm("X") = sector;
    register void *arg2 __asm("Y") = buffer;
    __asm volatile("hwi %0"
                   :
                   : "X"(data->mackapar),
                     "r"(action),
                     "r"(arg1),
                     "r"(arg2));
}

void mackapar_spin_down (Mackapar_driver_data *data) {
    if (data->type == MACKAPAR_M525HD) {
        register u16 action __asm("A") = ACTION_SPIN_DOWN;
        __asm volatile("hwi %0"
                       :
                       : "X"(data->mackapar),
                         "r"(action));
    }
}

void mackapar_spin_up (Mackapar_driver_data *data) {
    if (data->type == MACKAPAR_M525HD) {
        register u16 action __asm("A") = ACTION_SPIN_UP;
        __asm volatile("hwi %0"
                       :
                       : "X"(data->mackapar),
                         "r"(action));
    }
}
