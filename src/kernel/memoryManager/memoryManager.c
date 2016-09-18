#include "memoryManager.h"
#include "kernel/panic/panic.h"

/*
 * A memory block is setup like this : | OWNER | SIZE | DATA (or FREE if owner is 0xFFFF) |
 */

static u16 freeMem = TOTAL_MEMORY;

void mm_init() {
    KmallocHeader *p = (KmallocHeader *) MEMORY_START;
    *p = (KmallocHeader){
		.user = MEMORY_OWNER_FREE,
        .size = TOTAL_MEMORY,
    };
}

void *kmalloc(u16 owner, u16 size) {

	u16 realSize = size + 2;

	freeMem -= realSize;

	if(size > freeMem) {
		kpanic("Out of memory");
	}

    KmallocHeader *chunk = MEMORY_START;
    while(true) {
        if ((void*) chunk >= MEMORY_END) {
            kpanic("Out of memory");
        }

		if (chunk->user == MEMORY_OWNER_FREE && chunk->size == realSize) {
			chunk->user = owner;
			break;
		} else if (chunk->user == MEMORY_OWNER_FREE && chunk->size > realSize) {
			// We split the chunk in two
			chunk->data[size] = MEMORY_OWNER_FREE;
			chunk->data[size + 1] = chunk->size - realSize;

			// And now it's ours
			chunk->user = owner;
			chunk->size = size;

			break;
		}

		chunk = (KmallocHeader *) ((u16) chunk + chunk->size + 2);
    }

	for (u16 i = 0; i < size; ++i) {
		chunk->data[i] = 0;
	}

    return chunk->data;
}

void kfree(void *addr) {
    if (addr < MEMORY_START || addr > MEMORY_END) {
        return;
    }
    KmallocHeader *chunk = (addr - sizeof(KmallocHeader));
    chunk->user = MEMORY_OWNER_FREE;

    KmallocHeader *other;
    do {
		other = (KmallocHeader*) ((u16) chunk + chunk->size + 2);
		if(other->user == MEMORY_OWNER_FREE) {
			chunk->size += other->size + 2;
		}
	} while(other < (KmallocHeader *) MEMORY_END && other->user == MEMORY_OWNER_FREE);
}

void kfreePid(u16 pid) {
    KmallocHeader *chunk = (KmallocHeader *) MEMORY_START;

    u16 s = chunk->size;

    if(chunk->user == pid) {
        kfree(chunk);
    }

    chunk += s + 2;
}
