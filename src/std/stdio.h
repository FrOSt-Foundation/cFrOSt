#pragma once

#include "stdbool.h"
#include "types.h"

//OUTPUT

void printf (char *s);
void printc (char c);
void scroll (u16 lines);
void newline (void);
void move_cursor (u16 x, u16 y);
void clear (void);

//INPUT

char getc (void);

// DRIVES

// Stdio_drives_list must be identical to Stdio_kdrives_list defined in src/kernel/stdio/stdio.h.
typedef struct {
    u16 n_drives;
    char **types;
} Stdio_drives_list;

Stdio_drives_list *lsdrives (void);
void *drive_read (u16 drive, u32 location, u16 length);
bool drive_write (u16 drive, u32 location, u16 length, u16 *data);
