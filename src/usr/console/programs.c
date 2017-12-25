#include "programs.h"
#include "std/stdio.h"
#include "std/stdlib.h"
#include "std/string.h"

void console_help (u16 UNUSED (n_arguments), char **UNUSED (arguments)) {
    printf ("Commands: help, about, echo, mem, ps, kill, lsdrives, dd, mkfs.bbfs");
}

void console_about (u16 UNUSED (n_arguments), char **UNUSED (arguments)) {
    printf ("cFrOSt, a program brought to you by l3nn0x, Yamakaky and Azeryfun!");
}

void console_echo (u16 n_arguments, char **arguments) {
    if (n_arguments != 0) {
        for (u16 i = 0; i < n_arguments; ++i) {
            printf (arguments[i]);
            printc (' ');
        }
    }
}

void console_mem (u16 UNUSED (n_arguments), char **UNUSED (arguments)) {
    char *buffer = (char *)malloc (6);

    u16 mem = get_free_memory ();
    uitoa (mem, buffer);
    printf (buffer);
    printf (" words free");

    free ((u16 *)buffer);
}

void console_ps (u16 UNUSED (n_arguments), char **UNUSED (arguments)) {
    char **processes;
    u16 *pids;
    u16 n_processes = get_processes_list (&processes, &pids);

    char *buffer = (char *)malloc (6);

    for (u16 i = 0; i < n_processes; ++i) {
        uitoa (pids[i], buffer);
        printf (buffer);
        printf (": ");
        printf (processes[i]);
        printf ("\n");
    }

    free ((u16 *)buffer);
    free ((u16 *)pids);
    for (u16 i = 0; i < n_processes; ++i) {
        free ((u16 *)processes[i]);
    }
    free ((u16 *)processes);
}

void console_kill (u16 n_arguments, char **arguments) {
    for (u16 i = 0; i < n_arguments; ++i) {
        kill (atoui (arguments[i]));
    }
}

void console_lsdrives (u16 UNUSED (n_arguments), char **UNUSED (arguments)) {
    char *buffer = (char *)malloc (6);

    Stdio_drives_list *list = lsdrives ();
    for (u16 i = 0; i < list->n_drives; ++i) {
        uitoa (i, buffer);
        printf (buffer);
        printf (": ");
        switch (list->types[i]) {
            case STDIO_DRIVE_TYPE_M35FD:
                printf ("M35FD (");
                break;
            case STDIO_DRIVE_TYPE_M525HD:
                printf ("M525HD (");
                break;
        }
        uitoa (list->length_sectors[i], buffer);
        printf (buffer);
        printf (" sec, fs: ");
        switch (list->filesystems[i]) {
            case FS_NONE:
                printf ("None");
                break;
            case FS_BBFS:
                printf ("BBFS");
                break;
        }
        printf (")\n");
    }

    free ((u16 *)buffer);
}

void console_dd (u16 n_arguments, char **arguments) {
    if (n_arguments != 3) {
        printf ("Usage:\ndd <input_drive> <output_drive> <n_sectors>");
        return;
    }

    u16 input, output, n_sectors;
    input = atoui (arguments[0]);
    output = atoui (arguments[1]);
    n_sectors = atoui (arguments[2]);
    printf ("Confirm copy from disk ");
    printf (arguments[0]);
    printf (" to disk ");
    printf (arguments[1]);
    printf (" for ");
    printf (arguments[2]);
    printf (" sectors? (y/n)\n");

    char c = getc ();
    if (c != 'y') {
        printf ("Aborting...\n");
        return;
    }

    // This is not the most efficient algorithm; there is a lot of overhead in the read and write functions, and it could be improved by reading all the sectors at once. However, the current uses the least momory since it only saves one buffer at a time.
    for (u16 i = 0; i < n_sectors; ++i) {
        u16 *input_sector = drive_read (input, i * 512, 512);

        drive_write (output, i * 512, 512, input_sector);

        free (input_sector);
    }
}

void console_mkfs_bbfs (u16 n_arguments, char **arguments) {
    if (n_arguments != 1) {
        printf ("Usage:\nmkfs.bbfs <drive>");
        return;
    }

    u16 drive = atoui (arguments[0]);

    Stdio_drives_list *list = lsdrives ();
    if (drive >= list->n_drives) {
        printf ("Error: No such drive.");
        return;
    }

    printf ("Confirm you want to format disk ");
    printf (arguments[0]);
    printf ("? (y/n)\n");

    char c = getc ();
    if (c != 'y') {
        printf ("Aborting...\n");
        return;
    } else {
        printf ("Formatting... ");
    }

    bool success = drive_format (FS_BBFS, drive);
    if (!success) {
        printf ("Error!");
    } else {
        printf ("Success!");
    }
}

void console_ls (u16 n_arguments, char ** arguments) {
    if (n_arguments != 1) {
        printf ("Usage:\nls <path>");
        return;
    }

    char *buffer = (char *)malloc (6);

    char** files;
    i16 n_files = ls(arguments[0], &files);

    if (n_files >= 0) {
        uitoa((u16) n_files, buffer);
        printf(buffer);

        printf(" files:\n");
        for (u16 i = 0; i < (u16) n_files; ++i) {
            printf(files[i]);
            free((u16 *) files[i]);
        }
    } else {
        printf("Error");
    }

    free((u16 *)buffer);
    free((u16 *)files);
}

void console_no_such_command (u16 UNUSED (n_arguments), char **UNUSED (arguments)) {
    printf ("Error: No such command.");
}
