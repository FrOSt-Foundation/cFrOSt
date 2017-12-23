#pragma once

#include "types.h"

void memcpy (const void *from, void *to, u16 size);
u16 get_free_memory(void);
void *malloc (u16 size);
void free (u16 *block);
void mclear (u16 *block);
u16 msize (u16 *block);
u16 *realloc (u16 *block, u16 new_size);
u16 kill (u16 pid);
void add_process (void *location, char *name);
u16 get_processes_list (char ***names, u16 **pids);
void yield (void);
