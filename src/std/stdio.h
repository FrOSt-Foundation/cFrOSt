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

// Stdio_drives_list, Stdio_fs and Stdio_drive_type must be identical to Stdio_kdrives_list, Stdio_kfs and Stdio_kdrive_type defined in src/kernel/stdio/stdio.h.
typedef enum {
    STDIO_DRIVE_TYPE_M35FD,
    STDIO_DRIVE_TYPE_M525HD
} Stdio_drive_type;

typedef enum {
    FS_NONE,
    FS_BBFS
} Stdio_fs;

typedef struct {
    u16 n_drives;
    Stdio_drive_type *types;
    u16 *length_sectors;
    Stdio_fs *filesystems;
} Stdio_drives_list;

Stdio_drives_list *lsdrives (void);
void *drive_read (u16 drive, u32 location, u16 length);
bool drive_write (u16 drive, u32 location, u16 length, u16 *data);
bool drive_format(Stdio_fs filesystem, u16 drive);
