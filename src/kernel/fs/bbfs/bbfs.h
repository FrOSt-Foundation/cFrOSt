#pragma once

#include "drivers/mackapar/mackapar.h"
#include "kernel/stdio/stdio.h"
#include "types.h"

/*
 * DRIVES API
 */

typedef struct {
    u16 version;
    u16 *free_mask;
    u16 *fat;
} Bbfs_header;

typedef struct {
    u16 id;
    Bbfs_header *header;
    u16 n_sectors;
    u16 reserved_length;
} Bbfs_drive;

typedef struct {
    u16 n_drives;
    Bbfs_drive *drives;
} Bbfs_drives_list;

extern Bbfs_drives_list bbfs_drives_list;

typedef enum {
    BBFS_ERROR_NONE,
    BBFS_ERROR_WRONG_MODE,
    BBFS_ERROR_EOF,
    BBFS_ERROR_CORRUPTED_FS,
    BBFS_ERROR_INVALID,
    BBFS_ERROR_DISK,
    BBFS_ERROR_DISK_FULL,
} Bbfs_error_code;

/*
 * FILES API
 */

typedef u16 Bbfs_mode;
#define BBFS_MODE_READ ((Bbfs_mode)1)
#define BBFS_MODE_WRITE ((Bbfs_mode)2)

typedef struct {
    Bbfs_drive *drive;
    u16 root_sector; // First sector of the file
    Bbfs_mode mode;

    u16 sector;
    u16 offset;
} Bbfs_file;


void bbfs_init (void);
void bbfs_shutdown (void);
void bbfs_expand_list (void);
void bbfs_format (u16 drive);
bool bbfs_sync (Bbfs_drive *drive);

u16 bbfs_find_free_sector (Bbfs_drive *drive);

u16 bbfs_file_size (Bbfs_file *file);
Bbfs_file *bbfs_get_root_file (Bbfs_drive *drive);
Bbfs_error_code bbfs_seek (Bbfs_file *file, u16 distance);
Bbfs_error_code bbfs_read (Bbfs_file *file, u16 *d, u16 length);
Bbfs_error_code bbfs_write (Bbfs_file *file, u16 *f, u16 length);
Bbfs_error_code bbfs_create (Bbfs_drive *drive, Bbfs_file *file);
Bbfs_error_code bbfs_delete (Bbfs_file *file);

/*
 * DIRECTORIES API
 */

typedef enum {
    BBFS_FILE_TYPE_SUBDIRECTORY,
    BBFS_FILE_TYPE_FILE,
} Bbfs_file_type;

typedef struct {
    Bbfs_file_type type;
    u16 root_sector;
    u16 name[8];
} Bbfs_directory_entry_raw;

typedef struct {
    Bbfs_file_type type;
    Bbfs_file file;
    char name[17];
} Bbfs_directory_entry;

typedef struct {
    u16 version;
    u16 n_entries;
} Bbfs_directory_header_raw;

typedef struct {
    Bbfs_file *file;

    u16 version;
    u16 n_entries;
    Bbfs_directory_entry *entries;
} Bbfs_directory;

Bbfs_error_code bbfs_open_directory (Bbfs_file *file, Bbfs_directory *directory);
Bbfs_error_code bbfs_save_directory (Bbfs_directory *directory);
Bbfs_error_code bbfs_create_in_directory (Bbfs_directory *directory, Bbfs_file_type type, char name[], u16 name_length);
