#include "kernel/stdio/stdio.h"
#include "kernel/memoryManager/memoryManager.h"
#include "std/string.h"
#include "interruptHandler.h"
#include "asm.h"

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))

static void interruptHandler(u16);
static void stdio_printf_handler(u16 UNUSED(msg), u16 s, u16 UNUSED(arg2));
static void stdio_printc_handler(u16 UNUSED(msg), u16 c, u16 UNUSED(arg2));
static void stdio_scroll_handler(u16 UNUSED(msg), u16 lines, u16 UNUSED(arg2));
static void stdio_newline_handler(u16 UNUSED(msg), u16 UNUSED(arg1), u16 UNUSED(arg2));
static void stdio_moveCursor_handler(u16 UNUSED(msg), u16 x, u16 y);
static void stdio_clear_handler(u16 UNUSED(msg), u16 UNUSED(arg1), u16 UNUSED(arg2));
static void stdio_getc_handler(u16 UNUSED(msg), u16 ptr, u16 UNUSED(arg2));
static void mm_malloc_handler(u16, u16 size, u16 raw_ptr);
static void mm_free_handler(u16, u16 raw_ptr, u16 UNUSED(arg2));

static u16 int_table_size;
static IntHandler *int_table;

IntHandler *int_handler_allocate(u16 nb_hardware) {
    int_table_size = __SOFTINT_NB + nb_hardware;
    int_table = (IntHandler*)kmalloc(MEMORY_OWNER_KERNEL, int_table_size);
	int_table[SOFTINT_PRINTF] = stdio_printf_handler;
	int_table[SOFTINT_PRINTC] = stdio_printc_handler;
	int_table[SOFTINT_SCROLL] = stdio_scroll_handler;
	int_table[SOFTINT_NEWLINE] = stdio_newline_handler;
	int_table[SOFTINT_MOVECURSOR] = stdio_moveCursor_handler;
	int_table[SOFTINT_CLEAR] = stdio_clear_handler;
    int_table[SOFTINT_GETC] = stdio_getc_handler;
    int_table[SOFTINT_MALLOC] = mm_malloc_handler;
    int_table[SOFTINT_FREE] = mm_free_handler;
    return int_table + __SOFTINT_NB;
}

void int_handler_activate() {
    asm_ias((u16)&interruptHandler);
}

void interrupt1(u16 message, u16 arg1) {
    asm_int(message, arg1, 0);
}

void interrupt2(u16 message, u16 arg1, u16 arg2) {
    asm_int(message, arg1, arg2);
}

__attribute__ ((noinline,interrupt))
static void interruptHandler(u16 msg) {
    // the first argument is passed in A so it should be ok.
    // TODO: make sure it's OK.
    register u16 reg_b __asm("B");
    register u16 reg_c __asm("C");
    __asm volatile("": "=r"(reg_b), "=r"(reg_c));
    int_table[msg](msg, reg_b, reg_c);
}

static void stdio_printf_handler(u16 UNUSED(msg), u16 s, u16 UNUSED(arg2)) {
    stdio_printf((char*) s);
}

static void stdio_printc_handler(u16 UNUSED(msg), u16 c, u16 UNUSED(arg2)) {
    stdio_printc((char) c);
}

static void stdio_scroll_handler(u16 UNUSED(msg), u16 lines, u16 UNUSED(arg2)) {
    stdio_scroll(lines);
}

static void stdio_newline_handler(u16 UNUSED(msg), u16 UNUSED(arg1), u16 UNUSED(arg2)) {
    stdio_newline();
}

static void stdio_moveCursor_handler(u16 UNUSED(msg), u16 x, u16 y) {
	stdio_moveCursor(x, y);
}

static void stdio_clear_handler(u16 UNUSED(msg), u16 UNUSED(arg1), u16 UNUSED(arg2)) {
	stdio_clear();
}

static void stdio_getc_handler(u16 UNUSED(msg), u16 raw_ptr, u16 UNUSED(arg2)) {
	char *ptr = (char *) raw_ptr;

	*ptr = stdio_getc();
}

static void mm_malloc_handler(u16 UNUSED(msg), u16 size, u16 raw_ptr) {
    u16 *ptr = (u16*)(long)raw_ptr;
    *ptr = (u16)kmalloc(MEMORY_OWNER_UNKNOWN, size);
}

static void mm_free_handler(u16 UNUSED(msg), u16 raw_ptr, u16 UNUSED(arg2)) {
    kfree((u16*)(long)raw_ptr);
}
