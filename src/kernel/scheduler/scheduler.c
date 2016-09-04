#include "scheduler.h"

#include "kernel/memoryManager/memoryManager.h"
#include "kernel/panic/panic.h"
#include "std/string.h"

u16 running_process = 0;
u16 nProcesses = 0;
Process **processes = NULL;

u16 __scheduler_buffer1 = 0;
u16 __scheduler_buffer2 = 0;

void scheduler_init() {
}

void scheduler_start(Driver* driver_clock) {
    clock_set_tickrate(driver_clock->devicesList.data[0], 2);
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
    process->pc = (u16) location;
    process->name = name;
    process->pid = pid;

    process->stack = kmalloc(pid, STACK_SIZE);
    process->sp = (u16) process->stack + STACK_SIZE - 1 - 10; // - 10 because of 10 registers (A -> J + EX)

    processes[nProcesses - 1] = process;

    pid++;
}

// You can use PID 0 to kill your own program
u16 scheduler_kill(u16 pid) {
    if(nProcesses == 1) {
        kpanic("Last process killed");
    }

    if(pid == 0)
        pid = processes[running_process]->pid;

    for(u16 i = 0; i < nProcesses; ++i) {
        if(processes[i]->pid == pid) {

            // We free all allocated memory owned by the killed process
            kfree(processes[i]->stack);
            kfree(processes[i]);
            kfreePid(pid);

            // We re allocate the process table
            Process** oldProcesses = processes;
            processes = kmalloc(0, nProcesses - 1);
            for(u16 j = 0; j < i; ++j) {
                processes[j] = oldProcesses[j];
            }
            for(u16 j = i + 1; j < nProcesses; ++j) {
                processes[j - 1] = oldProcesses[j];
            }
            kfree(oldProcesses);

            nProcesses--;

            // If the process being killed is the current process
            if(i == running_process) {
                running_process = 0; // If we don't do that, if we kill the last processes scheduler_switch will not be able to set running_process to 0. Yes, it means that some processes may not get their turn immediately. Such is life!
                scheduler_abort();
            }

            // If the current process changed index, update it
            if(running_process > i) {
                running_process--;
            }

            return true;
        }
    }

    return false;
}

// Returns the number of processes, the list of PIDs in *p and the list of process names in *n
u16 scheduler_getProcessesList(char*** n, u16** p) {
    char** names = kmalloc(processes[running_process]->pid, nProcesses);
    u16* pids = kmalloc(processes[running_process]->pid, nProcesses);
    for(u16 i = 0; i < nProcesses; ++i) {
        names[i] = kmalloc(processes[running_process]->pid, strlen(processes[i]->name) + 1);
        strcpy(names[i], processes[i]->name);

        pids[i] = processes[i]->pid;
    }
    *n = names;
    *p = pids;

    return nProcesses;
}
