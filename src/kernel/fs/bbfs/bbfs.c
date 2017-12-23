#include "bbfs.h"

#include "kernel/panic/panic.h"

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
            .n_sectors = n_sectors
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
        drive->header = kmalloc (0, sizeof (Bbfs_header));
        drive->header->free_mask = kmalloc (0, drive->n_sectors / 16);
        drive->header->fat = kmalloc (0, drive->n_sectors);
    }

    // Set FAT and free mask to free
    drive->header->version = 0xBF55;
    for (u16 i = 0; i < drive->n_sectors / 16; ++i) {
        drive->header->free_mask[i] = 0xFFFF;
    }
    for (u16 i = 0; i < drive->n_sectors; ++i) {
        drive->header->fat[i] = 0xFFFF;
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
