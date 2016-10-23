#include "interrupt_handler.h"
#include "asm.h"
#include "kernel/memory_manager/memory_manager.h"
#include "kernel/scheduler/scheduler.h"
#include "kernel/stdio/stdio.h"
#include "std/string.h"

#include "types.h"

void interrupt_handler_asm (); // Declared in interrupt_handler.dasm
static void stdio_printf_handler (u16 UNUSED (msg), u16 s, u16 UNUSED (arg2), u16 UNUSED (arg3));
static void stdio_printc_handler (u16 UNUSED (msg), u16 c, u16 UNUSED (arg2), u16 UNUSED (arg3));
static void stdio_scroll_handler (u16 UNUSED (msg), u16 lines, u16 UNUSED (arg2), u16 UNUSED (arg3));
static void stdio_newline_handler (u16 UNUSED (msg), u16 UNUSED (arg1), u16 UNUSED (arg2), u16 UNUSED (arg3));
static void stdio_move_cursor_handler (u16 UNUSED (msg), u16 x, u16 y, u16 UNUSED (arg3));
static void stdio_clear_handler (u16 UNUSED (msg), u16 UNUSED (arg1), u16 UNUSED (arg2), u16 UNUSED (arg3));
static void stdio_getc_handler (u16 UNUSED (msg), u16 raw_ptr, u16 UNUSED (arg2), u16 UNUSED (arg3));
static void mm_malloc_handler (u16 UNUSED (msg), u16 size, u16 raw_ptr, u16 UNUSED (arg3));
static void mm_free_handler (u16 UNUSED (msg), u16 raw_ptr, u16 UNUSED (arg2), u16 UNUSED (arg3));
static void scheduler_kill_handler (u16 UNUSED (msg), u16 pid, u16 return_value, u16 UNUSED (arg3));
static void scheduler_yield_handler (u16 UNUSED (msg), u16 UNUSED (arg1), u16 UNUSED (arg2), u16 UNUSED (arg3));
static void scheduler_lsdrives_handler (u16 UNUSED (msg), u16 drives_list, u16 UNUSED (arg2), u16 UNUSED (arg3));
static void scheduler_driveread_handler (u16 UNUSED (msg), u16 drive_read_arguments, u16 UNUSED (arg2), u16 UNUSED (arg3));
static void scheduler_drivewrite_handler (u16 UNUSED (msg), u16 drive_write_arguments, u16 UNUSED (arg2), u16 UNUSED (arg3));
static void scheduler_add_process_handler (u16 UNUSED (msg), u16 location, u16 name, u16 UNUSED (arg3));
static void scheduler_get_processes_list_handler (u16 UNUSED (msg), u16 raw_ptr1, u16 raw_ptr2, u16 raw_ptr3);

u16 int_table_size;
static Int_handler *int_table;

Int_handler *int_handler_allocate (u16 nb_hardware) {
    int_table_size = __SOFTINT_NB + nb_hardware;
    int_table = (Int_handler *)kmalloc (MEMORY_OWNER_KERNEL, int_table_size);
    int_table[SOFTINT_PRINTF] = stdio_printf_handler;
    int_table[SOFTINT_PRINTC] = stdio_printc_handler;
    int_table[SOFTINT_SCROLL] = stdio_scroll_handler;
    int_table[SOFTINT_NEWLINE] = stdio_newline_handler;
    int_table[SOFTINT_MOVECURSOR] = stdio_move_cursor_handler;
    int_table[SOFTINT_CLEAR] = stdio_clear_handler;
    int_table[SOFTINT_GETC] = stdio_getc_handler;
    int_table[SOFTINT_MALLOC] = mm_malloc_handler;
    int_table[SOFTINT_FREE] = mm_free_handler;
    int_table[SOFTINT_KILL] = scheduler_kill_handler;
    int_table[SOFTINT_YIELD] = scheduler_yield_handler;
    int_table[SOFTINT_LSDRIVES] = scheduler_lsdrives_handler;
    int_table[SOFTINT_DRIVEREAD] = scheduler_driveread_handler;
    int_table[SOFTINT_DRIVEWRITE] = scheduler_drivewrite_handler;
    int_table[SOFTINT_ADDPROCESS] = scheduler_add_process_handler;
    int_table[SOFTINT_GET_PROCESSES_LIST] = scheduler_get_processes_list_handler;
    return int_table + __SOFTINT_NB;
}

void int_handler_activate () {
    asm_ias ((u16)&interrupt_handler_asm);
}

void interrupt (u16 message, u16 arg1, u16 arg2, u16 arg3) {
    asm_int (message, arg1, arg2, arg3);
}

__attribute__ ((noinline, interrupt)) static void interrupt_handler (u16 msg) {
    // the first argument is passed in A so it should be ok.
    // TODO: make sure it's OK.
    register u16 reg_b __asm("B");
    register u16 reg_c __asm("C");
    register u16 reg_x __asm("X");
    __asm volatile(""
                   : "=r"(reg_b), "=r"(reg_c), "=r"(reg_x));
    int_table[msg](msg, reg_b, reg_c, reg_x);
}

static void stdio_printf_handler (u16 UNUSED (msg), u16 s, u16 UNUSED (arg2), u16 UNUSED (arg3)) {
    stdio_printf ((char *)s);
}

static void stdio_printc_handler (u16 UNUSED (msg), u16 c, u16 UNUSED (arg2), u16 UNUSED (arg3)) {
    stdio_printc ((char)c);
}

static void stdio_scroll_handler (u16 UNUSED (msg), u16 lines, u16 UNUSED (arg2), u16 UNUSED (arg3)) {
    stdio_scroll (lines);
}

static void stdio_newline_handler (u16 UNUSED (msg), u16 UNUSED (arg1), u16 UNUSED (arg2), u16 UNUSED (arg3)) {
    stdio_newline ();
}

static void stdio_move_cursor_handler (u16 UNUSED (msg), u16 x, u16 y, u16 UNUSED (arg3)) {
    stdio_move_cursor (x, y);
}

static void stdio_clear_handler (u16 UNUSED (msg), u16 UNUSED (arg1), u16 UNUSED (arg2), u16 UNUSED (arg3)) {
    stdio_clear ();
}

static void stdio_getc_handler (u16 UNUSED (msg), u16 raw_ptr, u16 UNUSED (arg2), u16 UNUSED (arg3)) {
    char *ptr = (char *)raw_ptr;

    *ptr = stdio_getc ();
}

static void mm_malloc_handler (u16 UNUSED (msg), u16 size, u16 raw_ptr, u16 UNUSED (arg3)) {
    u16 *ptr = (u16 *)(long)raw_ptr;
    *ptr = (u16)kmalloc (MEMORY_OWNER_UNKNOWN, size);
}

static void mm_free_handler (u16 UNUSED (msg), u16 raw_ptr, u16 UNUSED (arg2), u16 UNUSED (arg3)) {
    kfree ((u16 *)(long)raw_ptr);
}

static void scheduler_kill_handler (u16 UNUSED (msg), u16 pid, u16 raw_ptr, u16 UNUSED (arg3)) {
    u16 *return_value = (u16 *)raw_ptr;
    *return_value = scheduler_kill (pid);
}

static void scheduler_yield_handler (u16 UNUSED (msg), u16 UNUSED (arg1), u16 UNUSED (arg2), u16 UNUSED (arg3)) {
    scheduler_yield ();
}

static void scheduler_lsdrives_handler (u16 UNUSED (msg), u16 drives_list, u16 UNUSED (arg2), u16 UNUSED (arg3)) {
    Stdio_kdrives_list **list = (Stdio_kdrives_list **)drives_list;
    *list = stdio_drives_list;
}

static void scheduler_driveread_handler (u16 UNUSED (msg), u16 drive_read_arguments, u16 UNUSED (arg2), u16 UNUSED (arg3)) {
    Drive_read_arguments *arguments = (Drive_read_arguments *)drive_read_arguments;
    *arguments->return_location = stdio_drive_read (arguments->drive, arguments->location, arguments->length);
}

static void scheduler_drivewrite_handler (u16 UNUSED (msg), u16 drive_write_arguments, u16 UNUSED (arg2), u16 UNUSED (arg3)) {
    Drive_write_arguments *arguments = (Drive_write_arguments *)drive_write_arguments;
    *arguments->return_value = stdio_drive_write (arguments->drive, arguments->location, arguments->length, arguments->data);
}

static void scheduler_add_process_handler (u16 UNUSED (msg), u16 location, u16 name, u16 UNUSED (arg3)) {
    scheduler_add_process ((void *)location, (char *)name);
}

static void scheduler_get_processes_list_handler (u16 UNUSED (msg), u16 raw_ptr1, u16 raw_ptr2, u16 raw_ptr3) {
    u16 *n = (u16 *)raw_ptr1;
    char ***names = (char ***)raw_ptr2;
    u16 **pids = (u16 **)raw_ptr3;
    *n = scheduler_get_processes_list (names, pids);
}
