#include "panic.h"
#include "kernel/stdio/stdio.h"

__attribute__ ((noreturn))
void kpanic(char *data) {
	stdio_init_output(lem1802, &driver_lem1802);
	for(u16 i = 0; i < driver_lem1802.devicesList.nDevices; ++i) {
		stdio_set_current_output(i);
		stdio_clear();
		stdio_printf("KERNEL PANIC\n");
		stdio_printf(data);
	}

	asm_brk(0);
	asm_iaq(1);

    // TODO: replace by HLT when supported
    while(1) {

    }
}
