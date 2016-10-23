#pragma once

#include "drivers/hardware.h"
#include "kernel/interrupt_handler/interrupt_handler.h"
#include "stdbool.h"
#include "types.h"

typedef enum {
    MACKAPAR_POLL_DEVICE,
    MACKAPAR_SET_INTERRUPT,
    MACKAPAR_READ_SECTOR,
    MACKAPAR_WRITE_SECTOR,
    MACKAPAR_SPIN_DOWN,
    MACKAPAR_SPIN_UP
} Mackapar_message;

typedef enum {
    MACKAPAR_M35FD,
    MACKAPAR_M525HD
} Mackapar_type;

typedef enum {
    STATE_NO_MEDIA = 0,
    STATE_READY = 1,
    STATE_READY_WP = 2,
    STATE_BUSY = 3,
    STATE_PARKED = 4,
    STATE_PARKED_WP = 5,
    STATE_INIT = 6,
    STATE_INIT_WP = 7
} Mackapar_state;

typedef enum {
    ERROR_NONE = 0,
    ERROR_BUSY = 1,
    ERROR_BAD_ADDRESS = 2,
    ERROR_NO_MEDIA = 2,
    ERROR_PROTECTED = 3,
    ERROR_PARKED = 4,
    ERROR_EJECT = 4,
    ERROR_BAD_SECTOR = 5,
    ERROR_BROKEN = (int)(0xFFFF)
} Mackapar_error;

typedef struct {
    u16 mackapar;
    Mackapar_type type;
} Mackapar_driver_data;

extern Driver driver_m35fd;
extern Driver driver_m525hd;

#define WORDS_PER_SECTOR 512

void *mackapar_init_m35fd (u16 mackapar, u16 UNUSED (int_number), Int_handler *UNUSED (int_handler_location));
void *mackapar_init_m525hd (u16 mackapar, u16 UNUSED (int_number), Int_handler *UNUSED (int_handler_location));
void *mackapar_init (u16 mackapar, Mackapar_type type);
void mackapar_destroy (void *UNUSED (data));
u16 mackapar_update_function (void *data, u16 message, u16 arg1, u16 arg2);

bool mackapar_write (Mackapar_driver_data *data, u32 location, u16 length, u16 *d);
void *mackapar_read (Mackapar_driver_data *data, u32 location, u16 length);

void mackapar_wait_until_ready (Mackapar_driver_data *data);
void mackapar_poll_device (Mackapar_driver_data *data, Mackapar_state *state, Mackapar_error *error);
void mackapar_set_interrupt (Mackapar_driver_data *data, u16 interrupt);
void mackapar_read_sector (Mackapar_driver_data *data, u16 sector, void *buffer);
void mackapar_write_sector (Mackapar_driver_data *data, u16 sector, void *buffer);
void mackapar_spin_down (Mackapar_driver_data *data);
void mackapar_spin_up (Mackapar_driver_data *data);
