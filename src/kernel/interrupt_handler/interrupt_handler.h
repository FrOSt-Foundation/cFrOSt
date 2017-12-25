#pragma once

#include "kernel/memory_manager/memory_manager.h"
#include "stdbool.h"

typedef enum {
    SOFTINT_PRINTF,
    SOFTINT_PRINTC,
    SOFTINT_SCROLL,
    SOFTINT_NEWLINE,
    SOFTINT_MOVECURSOR,
    SOFTINT_CLEAR,
    SOFTINT_GETC,
    SOFTINT_GET_FREE_MEMORY,
    SOFTINT_MALLOC,
    SOFTINT_FREE,
    SOFTINT_ADDPROCESS,
    SOFTINT_KILL,
    SOFTINT_YIELD,
    SOFTINT_LSDRIVES,
    SOFTINT_DRIVEREAD,
    SOFTINT_DRIVEWRITE,
    SOFTINT_FORMAT_BBFS,
    SOFTINT_LS,
    SOFTINT_GET_PROCESSES_LIST,
    __SOFTINT_NB,
} Soft_int;

typedef void (*Int_handler) (u16 message, u16 arg1, u16 arg2, u16 arg3);

extern u16 int_table_size;

Int_handler *int_handler_allocate (u16 nb_hardware);
void int_handler_activate (void);
void interrupt (u16 message, u16 arg_1, u16 arg_2, u16 arg_3);
// We don't reference interrupt_handler() because it shouldn't ever be called directly.


typedef struct {
    void **return_location;
    u16 drive;
    u32 location;
    u16 length;
} Drive_read_arguments;

typedef struct {
    bool *return_value;
    u16 drive;
    u32 location;
    u16 length;
    u16 *data;
} Drive_write_arguments;
