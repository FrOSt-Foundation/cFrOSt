
#ifndef _MODULE_H_
# define _MODULE_H_

struct Hardware_info;

typedef struct Module {
    Hardware_info *hw;
    void *data;

    void (*init)(struct Module *self);
    void (*clean)(struct Module *self);
} Module;

Module *module_create(Hardware_info *hw, void (*init)(Module*), void (*clean)(Module*));
void module_delete(Module *module);

#endif /* !_MODULE_H_ */
