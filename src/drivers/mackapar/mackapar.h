#include "drivers/hardware.h"
#include "kernel/interrupt_handler/interrupt_handler.h"
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

typedef struct {
    u16 mackapar;
    Mackapar_type type;
} Mackapar_driver_data;

extern Driver driver_m35fd;
extern Driver driver_m525hd;


void *mackapar_init_m35fd (u16 mackapar, u16 UNUSED (int_number), Int_handler *UNUSED (int_handler_location));
void *mackapar_init_m525hd (u16 mackapar, u16 UNUSED (int_number), Int_handler *UNUSED (int_handler_location));
void *mackapar_init (u16 mackapar, Mackapar_type type);
void mackapar_destroy (void *UNUSED (data));
u16 mackapar_update_function (void *data, u16 message, u16 arg1, u16 arg2);

void mackapar_poll_device (Mackapar_driver_data *data, u16 *state, u16 *error);
void mackapar_set_interrupt (Mackapar_driver_data *data, u16 interrupt);
void mackapar_read_sector (Mackapar_driver_data *data, u16 sector, void *buffer);
void mackapar_write_sector (Mackapar_driver_data *data, u16 sector, void *buffer);
void mackapar_spin_down (Mackapar_driver_data *data);
void mackapar_spin_up (Mackapar_driver_data *data);
