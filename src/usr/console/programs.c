#include "programs.h"
#include "std/stdio.h"
#include "std/stdlib.h"
#include "std/string.h"

void console_help (u16 UNUSED (n_arguments), char **UNUSED (arguments)) {
    printf ("Commands: help, about, echo, ps, kill");
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

void console_no_such_command (u16 UNUSED (n_arguments), char **UNUSED (arguments)) {
    printf ("Error: No such command.");
}
