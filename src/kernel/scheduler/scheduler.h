#pragma once

#include "drivers/hardware.h"
#include "types.h"

#define STACK_SIZE 0x100

typedef struct {
    u16 sp;
    u16 *stack;
    char *name;
    u16 pid;
} Process;

extern u16 running_process;
extern u16 n_processes;
extern Process **processes;

void scheduler_start (Driver *driver_clock);
void scheduler_add_process (void *location, char *name);
void scheduler_switch (void);
void scheduler_abort (void);
u16 scheduler_kill (u16 pid);
void scheduler_yield (void);
u16 scheduler_get_processes_list (char ***names, u16 **pids);
