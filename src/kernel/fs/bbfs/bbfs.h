#pragma once

#include "kernel/stdio/stdio.h"
#include "types.h"

typedef struct {
    u16 version;
    u16 *free_mask;
    u16 *fat;
} Bbfs_header;

typedef struct {
    u16 id;
    Bbfs_header *header;
    u16 n_sectors;
} Bbfs_drive;

typedef struct {
    u16 n_drives;
    Bbfs_drive *drives;
} Bbfs_drives_list;

extern Bbfs_drives_list bbfs_drives_list;

void bbfs_init (void);
void bbfs_shutdown (void);
void bbfs_expand_list(void);
void bbfs_format (u16 drive);
bool bbfs_sync (Bbfs_drive *drive);
