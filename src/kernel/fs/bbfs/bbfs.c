#include "bbfs.h"

#include "kernel/panic/panic.h"

// A sector is the last of its file if its high bit is set
#define BBFS_END_OF_FILE_SENTINEL 0x8000

/*
 * DRIVES API
 */

Bbfs_drives_list bbfs_drives_list = {
    .n_drives = 0,
    .drives = NULL
};

void bbfs_init (void) {
    if (stdio_drives_list->n_drives == 0) {
        return;
    }

    for (u16 drive = 0; drive < stdio_drives_list->n_drives; ++drive) {
        u16 n_sectors = stdio_drives_list->length_sectors[drive];

        // We read the BBFS version into a fresh struct to minimize kmallocs.
        Bbfs_header *bbfs_header = (Bbfs_header *)stdio_drive_read (drive, 512, sizeof (Bbfs_header));

        if (bbfs_header->version != 0xBF55) {
            kfree (bbfs_header);
            continue;
        }

        bbfs_header->free_mask = stdio_drive_read (drive, 512 + 6, n_sectors / 16);
        bbfs_header->fat = stdio_drive_read (drive, 512 + 6 + n_sectors / 16, n_sectors);

        if (bbfs_drives_list.n_drives == 1) {
            bbfs_drives_list.n_drives++;
            bbfs_drives_list.drives = kmalloc (0, sizeof (Bbfs_drive));
        } else {
            bbfs_expand_list ();
        }

        Bbfs_drive bbfs_drive = {
            .id = drive,
            .header = bbfs_header,
            .n_sectors = n_sectors,
            .reserved_length = 2 + ((n_sectors / 16 + n_sectors) / WORDS_PER_SECTOR + ((n_sectors / 16 + n_sectors) % WORDS_PER_SECTOR == 0 ? 0 : 1)),
        };
        bbfs_drives_list.drives[bbfs_drives_list.n_drives - 1] = bbfs_drive;

        stdio_drives_list->filesystems[drive] = KFS_BBFS;
    }
}

void bbfs_shutdown (void) {
    for (u16 i = 0; i < bbfs_drives_list.n_drives; ++i) {
        bbfs_sync (&bbfs_drives_list.drives[i]);
    }
}

// Returns new size
void bbfs_expand_list (void) {
    bbfs_drives_list.n_drives++;
    Bbfs_drive *old_drives = bbfs_drives_list.drives;
    bbfs_drives_list.drives = kmalloc (0, bbfs_drives_list.n_drives * sizeof (Bbfs_drive));
    for (u16 i = 0; i < (bbfs_drives_list.n_drives - 1); i++) {
        bbfs_drives_list.drives[i] = old_drives[i];
    }

    kfree (old_drives);
}

void bbfs_format (u16 d) {
    // Find out if a BBFS structure already exists for this drive
    bool drive_already_formatted = false;
    Bbfs_drive *drive = NULL;
    for (u16 i = 0; i < bbfs_drives_list.n_drives; ++i) {
        if (bbfs_drives_list.drives[i].id == d) {
            drive_already_formatted = true;
            drive = &bbfs_drives_list.drives[i];
        }
    }

    // Allocate new structures
    if (!drive_already_formatted) {
        bbfs_expand_list ();
        drive = &bbfs_drives_list.drives[bbfs_drives_list.n_drives - 1];
        drive->id = d;
        drive->n_sectors = stdio_drives_list->length_sectors[d];
        drive->reserved_length = 2 + ((drive->n_sectors / 16 + drive->n_sectors) / WORDS_PER_SECTOR + ((drive->n_sectors / 16 + drive->n_sectors) % WORDS_PER_SECTOR == 0 ? 0 : 1)); // Fat + free mask + bootloader + first sector
        drive->header = kmalloc (0, sizeof (Bbfs_header));
        drive->header->free_mask = kmalloc (0, drive->n_sectors / 16);
        drive->header->fat = kmalloc (0, drive->n_sectors);
    }

    // Set FAT and free mask to free
    drive->header->version = 0xBF55;
    for (u16 i = 1; i < drive->n_sectors / 16; ++i) {
        drive->header->free_mask[i] = 0xFFFF;
    }
    for (u16 i = 1; i < drive->n_sectors; ++i) {
        drive->header->fat[i] = 0xFFFF;
    }

    // ... Except for the boot sector / fat / root directory
    drive->header->free_mask[0] = 0xFFFF << drive->reserved_length;
    for (u16 i = 0; i < drive->reserved_length; ++i) {
        drive->header->fat[i] = 0x8000;
    }

    // We create the root directory
    Bbfs_directory_header_raw root_directory;
    root_directory.version = 1;
    root_directory.n_entries = 0;
    Bbfs_file *root_file = bbfs_get_root_file (drive);
    root_file->mode = BBFS_MODE_WRITE | BBFS_MODE_READ;
    Bbfs_error_code status = bbfs_write (root_file, (u16 *)&root_directory, sizeof (Bbfs_directory_header_raw));
    if (status != BBFS_ERROR_NONE) {
        asm_log (status);
        kpanic ("Could not create root directory");
    }

    // Create info.txt
    Bbfs_directory *directory = kmalloc (0, sizeof (Bbfs_directory));
    status = bbfs_open_directory (root_file, directory);
    if (status != BBFS_ERROR_NONE) {
        asm_log (status);
        kpanic ("Could not open root directory");
    }
    status = bbfs_create_in_directory (directory, BBFS_FILE_TYPE_FILE, "info.txt", 8);
    if (status != BBFS_ERROR_NONE) {
        asm_log (status);
        kpanic ("Could not create info.txt");
    }

    directory->entries[0].file.mode = BBFS_MODE_WRITE;
    status = bbfs_write (&directory->entries[0].file, (u16 *)"This file was created using FrOSt.", 35);
    if (status != BBFS_ERROR_NONE) {
        asm_log (status);
        kpanic ("Could not create info.txt");
    }
    kfree (directory->entries);
    kfree (directory);
    kfree (root_file);

    // Update drives list
    stdio_drives_list->filesystems[d] = KFS_BBFS;

    // Save to disk
    if (!bbfs_sync (drive))
        kpanic ("Could not sync new BBFS drive");
}

bool bbfs_sync (Bbfs_drive *drive) {
    if (!stdio_drive_write (drive->id, 512, 1, &drive->header->version) || !stdio_drive_write (drive->id, 512 + 6, drive->n_sectors / 16, drive->header->free_mask) || !stdio_drive_write (drive->id, 512 + 6 + drive->n_sectors / 16, drive->n_sectors, drive->header->fat))
        return false;
    return true;
}


/*
 * FILES API
 */

// Returns 0xFFFF if none could be found (disk full)
u16 bbfs_find_free_sector (Bbfs_drive *drive) {
    for (u16 i = 0; i < drive->n_sectors / 16; ++i) {
        if (drive->header->free_mask[i] != 0x0000) {
            for (u16 bit = 0; bit < 16; ++bit) {
                u16 mask = 1 << bit;

                if ((drive->header->free_mask[i] & mask) != 0)
                    return (i * 16) + bit;
            }
        }
    }

    return 0xFFFF;
}


u16 bbfs_file_size (Bbfs_file *file) {
    u16 sector = file->drive->header->fat[file->root_sector];
    u16 sectors_scanned = 0;

    if (sector == 0xFFFF)
        return 0;

    while ((sector & BBFS_END_OF_FILE_SENTINEL) == 0) {
        sector = file->drive->header->fat[sector];
        sectors_scanned++;
    }

    return (sectors_scanned * WORDS_PER_SECTOR) + (sector & 0x7FFF);
}

Bbfs_file *bbfs_get_root_file (Bbfs_drive *drive) {
    Bbfs_file *file = kmalloc (0, sizeof (Bbfs_file));
    file->drive = drive;
    file->root_sector = drive->reserved_length - 1;
    file->mode = BBFS_MODE_READ;
    file->sector = file->root_sector;
    file->offset = 0;

    return file;
}

Bbfs_error_code bbfs_seek (Bbfs_file *file, u16 distance) {
    file->offset = 0;
    file->sector = file->root_sector;

    while (file->offset + distance > WORDS_PER_SECTOR) {
        if ((file->sector & BBFS_END_OF_FILE_SENTINEL) != 0)
            return BBFS_ERROR_EOF;

        file->sector = file->drive->header->fat[file->sector];

        distance -= WORDS_PER_SECTOR;
    }

    if (file->sector & BBFS_END_OF_FILE_SENTINEL) { // If this is the last sector in the file
        u16 file_size = file->sector & 0x7FFF;
        if (file->offset + distance > file_size)
            return BBFS_ERROR_EOF;
    }

    file->offset += distance;

    return BBFS_ERROR_NONE;
}

Bbfs_error_code bbfs_read (Bbfs_file *file, u16 *d, u16 length) {
    if ((file->mode & BBFS_MODE_READ) == 0) {
        return BBFS_ERROR_WRONG_MODE;
    }

    while (length > 0) {
        if ((file->sector & BBFS_END_OF_FILE_SENTINEL) != 0) {
            u16 sector_length = file->sector & 0x7FFF;
            if (length > sector_length)
                return BBFS_ERROR_EOF;
        }

        u16 *read_bak = stdio_drive_read (file->drive->id, file->sector * WORDS_PER_SECTOR, WORDS_PER_SECTOR);
        u16 *read = read_bak;
        read += file->offset;
        for (u16 i = 0; i < (length >= WORDS_PER_SECTOR ? WORDS_PER_SECTOR : length); ++i) {
            *((u16 *)d) = *read;
            d++;
            read++;
        }
        kfree (read_bak);

        if (length >= WORDS_PER_SECTOR) {
            length -= length % WORDS_PER_SECTOR - file->offset;
            file->sector = file->drive->header->fat[file->sector];
            file->offset = 0;
        } else {
            file->offset += length;
            length = 0;
        }
    }

    return BBFS_ERROR_NONE;
}

Bbfs_error_code bbfs_write (Bbfs_file *file, u16 *from, u16 length) {
    if ((file->mode & BBFS_MODE_WRITE) == 0) {
        return BBFS_ERROR_WRONG_MODE;
    }

    if (file->offset + length > bbfs_file_size (file)) {
        u16 sectors_to_add = (length + file->offset) / WORDS_PER_SECTOR;

        if (sectors_to_add > 0) {
            for (u16 i = 0; i < sectors_to_add; ++i) { // We need to extend the file size
                u16 new_sector = bbfs_find_free_sector (file->drive);
                if (new_sector == 0xFFFF)
                    return BBFS_ERROR_DISK_FULL;

                file->drive->header->free_mask[new_sector / 16] |= 1 << new_sector % 16;
                file->drive->header->fat[file->sector + i] = new_sector;

                if (i == sectors_to_add - 1) {
                    file->drive->header->fat[new_sector] = 0x8000 | ((length + file->offset) % WORDS_PER_SECTOR);
                }
            }
        } else {
            file->drive->header->fat[file->sector] = 0x8000 | (length + file->offset);
        }

        if (!bbfs_sync (file->drive))
            return BBFS_ERROR_DISK;
    }

    while (length > 0) {
        if ((file->sector & BBFS_END_OF_FILE_SENTINEL) != 0) {
            u16 sector_length = file->sector & 0x7FFF;
            if (length > sector_length)
                kpanic ("Did not extend file correctly");
        }

        if (!stdio_drive_write (file->drive->id, file->sector * WORDS_PER_SECTOR + file->offset, length >= WORDS_PER_SECTOR ? WORDS_PER_SECTOR : length, from))
            return BBFS_ERROR_DISK;
        from += length >= WORDS_PER_SECTOR ? WORDS_PER_SECTOR : length;

        if (length >= WORDS_PER_SECTOR) {
            length -= length % WORDS_PER_SECTOR - file->offset;
            file->sector = file->drive->header->fat[file->sector];
            file->offset = 0;
        } else {
            file->offset += length;
            length = 0;
        }
    }

    return BBFS_ERROR_NONE;
}

Bbfs_error_code bbfs_create (Bbfs_drive *drive, Bbfs_file *file) {
    u16 sector = bbfs_find_free_sector (drive);
    if (sector == 0xFFFF)
        return BBFS_ERROR_DISK_FULL;

    drive->header->fat[sector] = BBFS_END_OF_FILE_SENTINEL;
    drive->header->free_mask[sector / 16] &= ~(1 << sector % 16);

    file->drive = drive;
    file->root_sector = sector;
    file->mode = BBFS_MODE_READ;
    file->sector = file->root_sector;
    file->offset = 0;

    if (!bbfs_sync (drive))
        return BBFS_ERROR_DISK;

    return BBFS_ERROR_NONE;
}

// Please note that deleting only deletes the reference from the FAT, not the file contents!
Bbfs_error_code bbfs_delete (Bbfs_file *file) {
    if (file->root_sector < file->drive->reserved_length)
        return BBFS_ERROR_INVALID;

    do {
        u16 next_sector = file->drive->header->fat[file->sector];

        file->drive->header->fat[file->sector] = 0xFFFF;
        file->drive->header->free_mask[file->sector / 16] |= 1 << file->sector % 16;

        file->sector = next_sector;
    } while ((file->sector & BBFS_END_OF_FILE_SENTINEL) == 0);

    if (!bbfs_sync (file->drive))
        return BBFS_ERROR_DISK;

    return BBFS_ERROR_NONE;
}


/*
 * DIRECTORIES API
 */

Bbfs_error_code bbfs_open_directory (Bbfs_file *file, Bbfs_directory *directory) {
    u16 size = bbfs_file_size (file);
    if (size == 0) {
        return BBFS_ERROR_INVALID;
    }

    u16 *buf = kmalloc (0, size);
    bbfs_read (file, buf, size);

    directory->file = file;
    directory->version = buf[0];
    directory->n_entries = buf[1];
    if (buf[1] != (size - 2) / sizeof (Bbfs_directory_entry_raw)) {
        return BBFS_ERROR_CORRUPTED_FS;
    }
    directory->entries = kmalloc (0, buf[1] * sizeof (Bbfs_directory_entry));

    for (u16 i = 0; i < buf[1]; ++i) {
        directory->entries[i].file.drive = file->drive;
        directory->entries[i].file.root_sector = buf[3 + i * sizeof (Bbfs_directory_entry)];
        directory->entries[i].file.mode = BBFS_MODE_READ;
        directory->entries[i].file.sector = directory->entries[i].file.root_sector;
        directory->entries[i].file.offset = 0;

        directory->entries[i].type = buf[2 + i * sizeof (Bbfs_directory_entry)];
        for (u16 j = 0; j < 8; ++j) {
            directory->entries[i].name[j * 2] = buf[4 + j + i * sizeof (Bbfs_directory_entry)] >> 8;
            directory->entries[i].name[j * 2 + 1] = buf[4 + j + i * sizeof (Bbfs_directory_entry)] & 0x00FF;
        }
    }

    kfree (buf);

    return BBFS_ERROR_NONE;
}

Bbfs_error_code bbfs_save_directory (Bbfs_directory *directory) {
    Bbfs_directory_header_raw directory_raw;

    directory_raw.version = directory->version;
    directory_raw.n_entries = directory->n_entries;

    Bbfs_directory_entry_raw *entries = kmalloc (0, directory_raw.n_entries * sizeof (Bbfs_directory_entry_raw));
    for (u16 i = 0; i < directory_raw.n_entries; ++i) {
        entries[i].type = directory->entries[i].type;
        entries[i].root_sector = directory->entries[i].file.root_sector;
        for (u16 j = 0; j < 8; ++j) {
            entries[i].name[j] = (u16) (directory->entries[i].name[2 * j] << 8 | directory->entries[i].name[2 * j + 1]);
        }
    }

    bbfs_seek (directory->file, 0);
    bbfs_write (directory->file, (u16 *)&directory_raw, sizeof (Bbfs_directory_header_raw));
    bbfs_write (directory->file, (u16 *)entries, directory->n_entries * sizeof (Bbfs_directory_entry_raw));

    kfree (entries);

    return BBFS_ERROR_NONE;
}

Bbfs_error_code bbfs_create_in_directory (Bbfs_directory *directory, Bbfs_file_type type, char name[], u16 name_length) {
    if (name_length > 16)
        return BBFS_ERROR_INVALID;

    directory->n_entries++;
    Bbfs_directory_entry *old_entries = directory->entries;
    directory->entries = kmalloc (0, directory->n_entries * sizeof (Bbfs_directory_entry));
    for (u16 i = 0; i < directory->n_entries - 1; ++i) {
        directory->entries[i] = old_entries[i];
    }
    kfree (old_entries);

    Bbfs_error_code status = bbfs_create (directory->file->drive, &directory->entries[directory->n_entries - 1].file);
    if (status != BBFS_ERROR_NONE) {
        return status;
    }

    directory->entries[directory->n_entries - 1].type = type;
    for (u16 i = 0; i < name_length; ++i) {
        directory->entries[directory->n_entries - 1].name[i] = name[i];
    }
    for (u16 i = name_length; i < 17; ++i) {
        directory->entries[directory->n_entries - 1].name[i] = 0;
    }

    return bbfs_save_directory (directory);
}
