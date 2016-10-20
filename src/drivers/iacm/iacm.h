#include "drivers/hardware.h"
#include "kernel/interrupt_handler/interrupt_handler.h"
#include "types.h"

typedef enum {
    IACM_SET_MODE,
    IACM_SET_RUN_TIME,
    IACM_SET_SLEEP_TIME,
    IACM_SET_INTERRUPT_MESSAGE,
    IACM_GET_CLOCK_RATE,
    IACM_GET_RCLOCK_RATE,
    IACM_RESET_SYSTEM
} Iacm_message;

typedef struct {
    u16 iacm;
} Iacm_driver_data;

extern Driver driver_iacm;


void *iacm_init (u16 iacm, u16 UNUSED (int_number), Int_handler *UNUSED (int_handler_location));
void iacm_destroy (void *UNUSED (data));
u16 iacm_update_function (void *data, u16 message, u16 arg1, u16 arg2);
void iacm_set_mode (Iacm_driver_data *data, u16 mode);
void iacm_set_run_time (Iacm_driver_data *data, u16 time);
void iacm_set_sleep_time (Iacm_driver_data *data, u16 time);
void iacm_set_interrupt_message (Iacm_driver_data *data, u16 message);
void iacm_get_clock_rate (Iacm_driver_data *data, u16 *rate_high, u16 *rate_low);
void iacm_get_rclock_rate (Iacm_driver_data *data, u16 *rate_high, u16 *rate_low);
void iacm_reset_system (Iacm_driver_data *data);
