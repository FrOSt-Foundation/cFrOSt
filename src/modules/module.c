#include "asm.h"
#include "types.h"
#include "kernel/memory_manager/memory_manager.h"
#include "modules/module.h"

Module *module_create(Hardware_info *hw, void (*init)(Module*), void (*clean)(Module*)) {
    Module *tmp = kmalloc(0, sizeof(*tmp));
    tmp->data = NULL;
    tmp->init = init;
    tmp->clean = clean;
    tmp->hw = hw;
    return tmp;
}

void module_delete(Module *module) {
    if (!module)
        return;
    module->clean(module);
    kfree(module);
}
