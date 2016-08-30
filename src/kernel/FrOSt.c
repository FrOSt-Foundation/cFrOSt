#include "types.h"
#include "asm.h"
#include "drivers/hardware.h"
#include "kernel/memoryManager/memoryManager.h"
#include "kernel/stdio/stdio.h"
#include "interruptHandler/interruptHandler.h"

#include "std/stdio.h"
#include "std/string.h"
#include "std/stdlib.h"

#include "usr/console/console.h"

int main(void) {
    mm_init();
    IntHandler *hardware_int_table = int_handler_allocate(asm_hwn());
    hardwareLoop(hardware_int_table);
    int_handler_activate();

	stdio_init_output(lem1802);
	stdio_init_input(generic_keyboard);

	console_main();

    while(1) {

    }
}
