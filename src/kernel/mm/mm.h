
#ifndef _MM_H_
# define _MM_H_

void *kmalloc(u16 owner, u16 size);
void kfree(void *ptr);

#endif /* !_MM_H_ */
