#pragma once

#define STACK_SIZE 0x100

typedef struct {
    u16 sp;
    u16* stack;
    char* name;
    u16 pid;
} Process;

extern u16 running_process;
extern u16 nProcesses;
extern Process **processes;

void scheduler_start(Driver* driver_clock);
void scheduler_addProcess();
void scheduler_switch();
void scheduler_abort();
u16 scheduler_kill(u16 pid);
u16 scheduler_getProcessesList(char*** names, u16** pids);