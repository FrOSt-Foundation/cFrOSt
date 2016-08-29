#include "types.h"
#include "asm.h"
#include "drivers/hardware.h"
#include "kernel/memoryManager/memoryManager.h"
#include "interruptHandler/interruptHandler.h"

#include "std/stdio.h"
#include "std/string.h"
#include "std/stdlib.h"

int main(void) {
    mm_init();
    IntHandler *hardware_int_table = int_handler_allocate(asm_hwn());
    hardwareLoop(hardware_int_table);
    int_handler_activate();

	stdio_init(lem1802);

    printf("cFrOSt loaded from disk, and interrupts enabled. Loading modules...\n");

    char* buffer = (char*) malloc(6);
	uitoa(12345, buffer);
    printf(buffer);
    free((u16*) buffer);

	printf("\nHello, world!");

    while(1) {

    }
}
