#include "types.h"
#include "kernel/memory_manager/memory_manager.h"
#include "modules/module.h"
#include "std/stdlib.h"
#include "std/string.h"

static Module *modules[20] = {0};

bool module_load(const char name[MODULE_LEN_NAME], void (*init)(void), void (*clean)(void)) {
    Module *tmp = kmalloc(0, sizeof(*tmp));
    tmp->init = init;
    tmp->clean = clean;
    memcpy(name, tmp->name, MODULE_LEN_NAME);
    tmp->init();
    for (u16 i = 0; i < sizeof(modules) / sizeof(modules[0]); ++i)
        if (!modules[i]) {
            modules[i] = tmp;
            return true;
        }
    kfree(tmp);
    return false;
}

static inline Module **module_find(const char name[MODULE_LEN_NAME]) {
    for (u16 i = 0; i < sizeof(modules) / sizeof(modules[0]); ++i) {
        if (!modules[i])
            continue;
        if (!strcmp(name, modules[i]->name))
            return &modules[i];
    }
    return NULL;
}

void module_unload(const char name[MODULE_LEN_NAME]) {
    Module **module = module_find(name);;
    if (!module)
        return;
    (*module)->clean();
    kfree(*module);
    *module = NULL;
}

void module_reload(const char name[MODULE_LEN_NAME]) {
    Module **module = module_find(name);
    if (!module)
        return;
    (*module)->clean();
    (*module)->init();
}

void module_delete() {
    for (u16 i = 0; i < sizeof(modules) / sizeof(modules[0]); ++i) {
        if (modules[i]) {
            modules[i]->clean();
            kfree(modules[i]);
            modules[i] = NULL;
        }
    }
}

char **module_get_names() {
    u16 nb = 0;
    for (u16 i = 0; i < sizeof(modules) / sizeof(modules[0]); ++i)
        nb += (modules[i] != 0);
    char **names = kmalloc(0, nb * sizeof(*names));
    for (u16 i = 0; i < sizeof(modules) / sizeof(modules[0]); ++i)
        if (modules[i])
        names[i] = strdup(modules[i]->name);
    return names;
}
