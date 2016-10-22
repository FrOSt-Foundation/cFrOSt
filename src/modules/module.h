
#ifndef _MODULE_H_
# define _MODULE_H_

#include "stdbool.h"

#define MODULE_LEN_NAME 8

typedef struct {
    void (*init)(void);
    void (*clean)(void);
    char name[MODULE_LEN_NAME];
} Module;

/*
 * Each module will call register functions as needed
 * for example if the want to be registered for an INT then in the init() they'll call interrupt_handler_register(INT_VALUE, &some_function);
 * Data structures are provided by the OS when necessary
 *
 */

bool module_load(const char name[MODULE_LEN_NAME], void (*init)(void), void (*clean)(void));
void module_unload(const char name[MODULE_LEN_NAME]);
void module_reload(const char name[MODULE_LEN_NAME]);
void module_delete();
char **module_get_names();

#endif /* !_MODULE_H_ */
