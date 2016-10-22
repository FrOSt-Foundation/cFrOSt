#include "stdlib.h"
#include "kernel/interrupt_handler/interrupt_handler.h"

void memcpy (const void *from, void *to, u16 size) {
    for (u16 i = 0; i < size; i++) {
        ((u16 *)to)[i] = ((u16 *)from)[i];
    }
}

void *malloc (u16 size) {
    u16 *p;
    interrupt (SOFTINT_MALLOC, size, (u16)&p, 0);
    return p;
}

void free (u16 *block) {
    interrupt (SOFTINT_FREE, (u16)block, 0, 0);
}

void add_process (void *location, char *name) {
    interrupt (SOFTINT_ADDPROCESS, (u16)location, (u16)name, 0);
}

u16 kill (u16 pid) {
    u16 v;
    interrupt (SOFTINT_KILL, pid, (u16)&v, 0);
    return v;
}

u16 get_processes_list (char ***names, u16 **pids) {
    u16 n;
    interrupt (SOFTINT_GET_PROCESSES_LIST, (u16)&n, (u16)names, (u16)pids);
    return n;
}
