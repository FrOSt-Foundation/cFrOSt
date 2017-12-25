#pragma once

#include "types.h"

void console_help (u16 UNUSED (n_arguments), char **UNUSED (arguments));
void console_about (u16 UNUSED (n_arguments), char **UNUSED (arguments));
void console_echo (u16 n_arguments, char **arguments);
void console_mem (u16 UNUSED (n_arguments), char **UNUSED (arguments));
void console_ps (u16 UNUSED (n_arguments), char **UNUSED (arguments));
void console_kill (u16 n_arguments, char **arguments);
void console_lsdrives (u16 UNUSED (n_arguments), char **UNUSED (arguments));
void console_dd (u16 n_arguments, char **arguments);
void console_mkfs_bbfs (u16 n_arguments, char **arguments);
void console_ls (u16 n_arguments, char ** arguments);
void console_no_such_command (u16 UNUSED (n_arguments), char **UNUSED (arguments));
