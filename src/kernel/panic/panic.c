#include "panic.h"
#include "kernel/stdio/stdio.h"

__attribute__ ((noreturn))
void kpanic(char *data) {
    (void)data;
	stdio_printf("\nKERNEL PANIC\n");
	stdio_printf(data);
    // TODO: replace by HLT when supported
    while(1) {

    }
}
