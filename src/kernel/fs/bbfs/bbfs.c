#include "bbfs.h"

#include "kernel/panic/panic.h"

// A sector is the last of its file if its high bit is set
#define BBFS_END_OF_FILE_SENTINEL 0x8000

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

        bbfs_drives_list.n_drives += 1;
        if (bbfs_drives_list.n_drives == 1) {
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

Bbfs_file bbfs_get_root_file(Bbfs_drive* drive) {
    Bbfs_file file;
    file.drive = drive;
    file.root_sector = drive->reserved_length - 1;
    file.mode = BBFS_MODE_READ;
    file.sector = file.root_sector;
    file.offset = 0;

    return file;
}

Bbfs_error_code bbfs_seek(Bbfs_file *file, u16 distance) {
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

Bbfs_error_code bbfs_read(Bbfs_file *file, u16* d, u16 length) {
    if ((file->mode & BBFS_MODE_READ) == 0) {
        return BBFS_ERROR_WRONG_MODE;
    }

    while (length > 0) {
        if ((file->sector & BBFS_END_OF_FILE_SENTINEL) != 0) {
            u16 sector_length = file->sector & 0x7FFF;
            if (length > sector_length)
                return BBFS_ERROR_EOF;
        }

        asm_log(0xcafe);
        asm_log(file->sector);
        asm_log(file->sector * WORDS_PER_SECTOR);
        u16* read = stdio_drive_read(file->drive->id, file->sector * WORDS_PER_SECTOR, WORDS_PER_SECTOR);
        read += file->offset;
        for(u16 i = 0; i < (length >= WORDS_PER_SECTOR ? WORDS_PER_SECTOR : length); ++i) {
            *((u16*)d) = *read;
            d++;
            read++;
        }
        kfree(read);

        if (length >= WORDS_PER_SECTOR) {
            length -= length % WORDS_PER_SECTOR - file->offset;
            file->sector = file->drive->header->fat[file->sector];
            file->offset = 0;
        } else {
            length = 0;
            file->offset += length;
        }
    }

    return BBFS_ERROR_NONE;
}
