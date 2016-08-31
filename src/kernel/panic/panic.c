#include "panic.h"
#include "kernel/stdio/stdio.h"

__attribute__ ((noreturn))
void kpanic(char *data) {
	stdio_init_output(lem1802, &driver_lem1802);
	for(u16 i = 0; i < driver_lem1802.nDevices; ++i) {
		stdio_set_current_output(i);
		stdio_printf("\nKERNEL PANIC\n");
		stdio_printf(data);
	}
    // TODO: replace by HLT when supported
    while(1) {

    }
}
