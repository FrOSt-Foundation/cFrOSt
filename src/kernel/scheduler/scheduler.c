#include "scheduler.h"

#include "kernel/memoryManager/memoryManager.h"

u16 running_process = 0;
u16 nProcesses = 0;
Process **processes = NULL;

void scheduler_init() {
}

void scheduler_start(Driver* driver_clock) {
    asm_log(processes[0]->stack);
    asm_log(processes[1]->stack);
    clock_set_tickrate(driver_clock->devicesList.data[0], 60);
    if(nProcesses == 0) {
        kpanic("No processes added to scheduler!");
    }

    __asm("\
    set SP, %0 \n \
    set PC, %1 \n \
    " :: "X" (processes[0]->sp), "X" (processes[0]->pc));
}

void scheduler_addProcess(void* location, char* name) {
    static u16 pid = 1;
    nProcesses++;

    // We (re-)allocate the processes table
    if(processes == NULL) {
        processes = kmalloc(0, 1);
    } else {
        Process** oldProcesses = processes;
        processes = kmalloc(0, nProcesses);
        for(u16 i = 0; i < nProcesses - 1; ++i) {
            processes[i] = oldProcesses[i];
        }
        kfree(oldProcesses);
    }

    Process *process = kmalloc(0, sizeof(Process));
    process->regs = (Registers) {
        .a = 0, .b = 0, .c = 0, .x = 0, .y = 0, .z = 0, .i = 0, .j = 0, .ex = 0
    };
    process->pc = location;
    process->name = name;

    process->stack = kmalloc(pid, STACK_SIZE);
    process->sp = (u16) process->stack + STACK_SIZE - 1;

    processes[nProcesses - 1] = process;
}
