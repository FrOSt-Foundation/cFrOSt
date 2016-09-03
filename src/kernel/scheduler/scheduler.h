#pragma once

#define STACK_SIZE 0x100

typedef struct {
    u16 a, b, c, x, y, z, i, j, ex;
} Registers;

typedef struct {
    Registers regs;
    u16 pc;
    u16 sp;
    void* stack;
    char* name;
} Process;

extern u16 running_process;
extern u16 nProcesses;
extern Process **processes;

void scheduler_init();
void scheduler_start(Driver* driver_clock);
void scheduler_addProcess();
void scheduler_switch();
