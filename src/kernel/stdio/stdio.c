#include "stdio.h"

#include "drivers/keyboard/keyboard.h"
#include "drivers/lem1802/lem1802.h"
#include "drivers/mackapar/mackapar.h"
#include "std/stdlib.h"
#include "types.h"

static Stdio_output_type output_type = no_output;
static Driver *output_driver, *input_driver;
static Stdio_input_type input_type = no_input;

static u16 **output_data;
static u16 current_output;
static u16 current_input;

char *type_m525hd = "m525hd";
char *type_m35fd = "m35fd";
Stdio_kdrives_list *stdio_drives_list;

// You can init stdio with whatever output device you want!
void stdio_init_output (Stdio_output_type t, Driver *d) {
    output_type = t;

    if (output_data != 0) {
        for (u16 i = 0; i < output_driver->devices_list.n_devices; ++i) {
            kfree (output_data[i]);
        }
        kfree (output_data);
    }

    output_driver = d;

    output_data = kmalloc (0, d->devices_list.n_devices);

    current_output = 0;

    switch (t) {
        case lem1802:
            for (u16 i = 0; i < d->devices_list.n_devices; ++i) {
                output_data[i] = kmalloc (0, __OUTPUTDATA_SIZE);
                output_data[i][ROWS] = 12;
                output_data[i][COLS] = 32;
                output_data[i][ROW] = 0;
                output_data[i][COL] = 0;
            }
            return;
        case no_output:
            break;
    }
}

void stdio_init_input (Stdio_input_type t, Driver *d) {
    input_type = t;

    input_driver = d;

    current_input = 0;

    switch (t) {
        case generic_keyboard:
            keyboard_clear_buffer (input_driver->devices_list.data[current_input]);
            return;
        case no_output:
            break;
    }
}

void stdio_set_current_output (u16 output_number) {
    current_output = output_number;
}

void stdio_scroll (u16 lines) {
    for (u16 line = 0; line < lines; ++line) {
        switch (output_type) {
            case lem1802:
                lem1802_scroll (output_driver->devices_list.data[current_output]);
                break;
            case no_output:
                break;
        }
    }
}

// Move cursor to a new row, at the first column.
void stdio_newline () {
    output_data[current_output][COL] = 0;
    if (output_data[current_output][ROW] == output_data[current_output][ROWS] - 1) {
        stdio_scroll (1);
    } else {
        output_data[current_output][ROW]++;
    }
}

void stdio_printc (char c) {
    if (output_data[current_output][COL] == output_data[current_output][COLS])
        stdio_newline ();

    stdio_putc (c);

    output_data[current_output][COL]++;
}

void stdio_putc (char c) {
    switch (output_type) {
        case lem1802:
            lem1802_putc (output_driver->devices_list.data[current_output], c, output_data[current_output][ROW] * 32 + output_data[current_output][COL]);
            break;
        case no_output:
            break;
    }
}

// Print a "formatted" string (only supports newline '\n' for now)
void stdio_printf (char *string) {
    char *p = string;
    while (*p != '\0') {
        switch (*p) {
            case 0xA:
                stdio_newline ();
                break;
            default:
                stdio_printc (*p);
        }

        p++;
    }
}

void stdio_move_cursor (u16 x, u16 y) {
    if (x < output_data[current_output][COLS])
        output_data[current_output][COL] = x;
    if (y < output_data[current_output][ROWS])
        output_data[current_output][ROW] = y;
}

void stdio_clear () {
    output_data[current_output][ROW] = 0;
    output_data[current_output][COL] = 0;

    switch (output_type) {
        case lem1802:
            lem1802_clear (output_driver->devices_list.data[current_output]);
            break;
        case no_output:
            break;
    }
}

char stdio_getc () {
    char c = '\0';
    switch (input_type) {
        case generic_keyboard:
            c = keyboard_get_next (input_driver->devices_list.data[current_input]);

            break;
        case no_input:
            break;
    }

    return c;
}


/*
 * Drive numbers are sequential. First, the m525hds, second, the m35fds. That way, we don't have to store a drives list, we can just find the mackapar data by reading the m525hds and m35fds Driver structs.
 */

void stdio_init_drives_list (void) {
    stdio_drives_list = kmalloc (0, sizeof (Stdio_kdrives_list));

    stdio_drives_list->n_drives = driver_m35fd.devices_list.n_devices + driver_m525hd.devices_list.n_devices;
    stdio_drives_list->types = kmalloc (0, driver_m35fd.devices_list.n_devices + driver_m525hd.devices_list.n_devices);

    for (u16 i = 0; i < driver_m525hd.devices_list.n_devices; ++i) {
        stdio_drives_list->types[i] = type_m525hd;
    }
    for (u16 i = driver_m525hd.devices_list.n_devices; i < driver_m525hd.devices_list.n_devices + driver_m35fd.devices_list.n_devices; ++i) {
        stdio_drives_list->types[i] = type_m35fd;
    }
}
