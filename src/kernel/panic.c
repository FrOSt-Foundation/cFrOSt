#include "panic.h"

__attribute__ ((noreturn))
void kpanic(const char *data) {
    (void)data;
    // TODO: replace by HLT when supported
    while(1) {

    }
}
