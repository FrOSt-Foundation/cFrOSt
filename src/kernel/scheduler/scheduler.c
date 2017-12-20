#include "scheduler.h"
#include <stdbool.h>

#include "drivers/clock/clock.h"
#include "drivers/iacm/iacm.h"
#include "kernel/memory_manager/memory_manager.h"
#include "kernel/panic/panic.h"
#include "std/string.h"

u16 running_process = 0;
u16 n_processes = 0;
Process **processes = NULL;

void scheduler_start_asm ();

void scheduler_start (Driver *clock) {
    clock_set_tickrate (clock->devices_list.data[0], 2);
    if (n_processes == 0) {
        kpanic ("No processes added to scheduler!");
    }

    scheduler_start_asm ();
}

void scheduler_add_process (void *location, char *name) {
    static u16 pid = 1;
    n_processes++;

    // We (re-)allocate the processes table
    if (processes == NULL) {
        processes = kmalloc (0, 1);
    } else {
        Process **old_processes = processes;
        processes = kmalloc (0, n_processes);
        for (u16 i = 0; i < n_processes - 1; ++i) {
            processes[i] = old_processes[i];
        }
        kfree (old_processes);
    }

    Process *process = kmalloc (0, sizeof (Process));
    process->name = name;
    process->pid = pid;

    process->stack = kmalloc (pid, STACK_SIZE);
    process->stack[STACK_SIZE - 2] = (u16)location;
    process->sp = (u16)process->stack + STACK_SIZE - 1 - 10; // - 10 because of 10 registers (A -> J + EX)

    processes[n_processes - 1] = process;

    pid++;
}

// You can use PID 0 to kill your own program
u16 scheduler_kill (u16 pid) {
    if (n_processes == 1) {
        kpanic ("Last process killed");
    }

    if (pid == 0)
        pid = processes[running_process]->pid;

    for (u16 i = 0; i < n_processes; ++i) {
        if (processes[i]->pid == pid) {

            // We free all allocated memory owned by the killed process
            kfree (processes[i]->stack);
            kfree (processes[i]);
            kfree_pid (pid);

            // We re allocate the process table
            Process **old_processes = processes;
            processes = kmalloc (0, n_processes - 1);
            for (u16 j = 0; j < i; ++j) {
                processes[j] = old_processes[j];
            }
            for (u16 j = i + 1; j < n_processes; ++j) {
                processes[j - 1] = old_processes[j];
            }
            kfree (old_processes);

            n_processes--;

            // If the process being killed is the current process
            if (i == running_process) {
                running_process = 0; // If we don't do that, if we kill the last processes scheduler_switch will not be able to set running_process to 0. Yes, it means that some processes may not get their turn immediately. Such is life!
                scheduler_abort ();
            }

            // If the current process changed index, update it
            if (running_process > i) {
                running_process--;
            }

            return true;
        }
    }

    return false;
}

void scheduler_yield (void) {
    if (n_processes == 1) {
        // asm_hlt (); // Uncomment when HLT fixed in emulator
    } else {
        asm_int (0xFFFE, 0, 0, 0);
    }
}

// Returns the number of processes, the list of PIDs in *p and the list of process names in *n
u16 scheduler_get_processes_list (char ***n, u16 **p) {
    char **names = kmalloc (processes[running_process]->pid, n_processes);
    u16 *pids = kmalloc (processes[running_process]->pid, n_processes);
    for (u16 i = 0; i < n_processes; ++i) {
        names[i] = kmalloc (processes[running_process]->pid, strlen (processes[i]->name) + 1);
        strcpy (names[i], processes[i]->name);

        pids[i] = processes[i]->pid;
    }
    *n = names;
    *p = pids;

    return n_processes;
}
