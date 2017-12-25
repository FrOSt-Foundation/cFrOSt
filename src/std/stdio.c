#include "stdio.h"
#include "stdlib.h"

#include "kernel/interrupt_handler/interrupt_handler.h"
#include "types.h"

/*
 * OUTPUT
 */

void printf (char *s) {
    interrupt (SOFTINT_PRINTF, (u16)s, 0, 0);
}

void printc (char c) {
    interrupt (SOFTINT_PRINTC, (u16)c, 0, 0);
}

void scroll (u16 lines) {
    interrupt (SOFTINT_SCROLL, lines, 0, 0);
}

void newline () {
    interrupt (SOFTINT_NEWLINE, 0, 0, 0);
}

void move_cursor (u16 x, u16 y) {
    interrupt (SOFTINT_MOVECURSOR, x, y, 0);
}

void clear () {
    interrupt (SOFTINT_CLEAR, 0, 0, 0);
}


/*
 * INPUT
 */

char getc () {
    char c;
    do {
        interrupt (SOFTINT_GETC, (u16)&c, 0, 0);
        if (c == '\0')
            yield ();
    } while (c == '\0');

    return c;
}

/*
 * DRIVES
 */

Stdio_drives_list *lsdrives (void) {
    Stdio_drives_list *list = 0;
    interrupt (SOFTINT_LSDRIVES, (u16)&list, 0, 0);
    return list;
}

void *drive_read (u16 drive, u32 location, u16 length) {
    void *return_value;
    Drive_read_arguments *arguments = (Drive_read_arguments *)malloc (sizeof (Drive_read_arguments));
    arguments->return_location = &return_value;
    arguments->drive = drive;
    arguments->location = location;
    arguments->length = length;

    interrupt (SOFTINT_DRIVEREAD, (u16)arguments, 0, 0);

    free ((u16 *)arguments);

    return return_value;
}

bool drive_write (u16 drive, u32 location, u16 length, u16 *data) {
    bool return_value;
    Drive_write_arguments *arguments = (Drive_write_arguments *)malloc (sizeof (Drive_write_arguments));
    arguments->return_value = &return_value;
    arguments->drive = drive;
    arguments->location = location;
    arguments->length = length;
    arguments->data = data;

    interrupt (SOFTINT_DRIVEWRITE, (u16)arguments, 0, 0);

    free ((u16 *)arguments);

    return return_value;
}

bool drive_format (Stdio_fs filesystem, u16 drive) {
    switch (filesystem) {
        case FS_BBFS:
            interrupt (SOFTINT_FORMAT_BBFS, drive, 0, 0);
            return true;
        case FS_NONE:
            return false;
    }
}

i16 ls (const char* path, char*** out) {
    i16 n = -1;
    interrupt(SOFTINT_LS, (u16) &n, (u16) path, (u16) out);
    return n;
}
