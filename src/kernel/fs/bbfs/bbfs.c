#include "bbfs.h"

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
        u16 header_length = 6 + n_sectors / 16 + n_sectors;
        u16 header_length_sectors = header_length / 512 + (header_length % 512 == 0 ? 0 : 1);

        // We read the BBFS version into a fresh struct to minimize kmallocs.
        Bbfs_header *bbfs_header = (Bbfs_header *)stdio_drive_read (drive, 512, sizeof (Bbfs_header));

        if (bbfs_header->version != 0xBF55) {
            continue;
        }

        bbfs_header->free_mask = stdio_drive_read (drive, 512 + 6, n_sectors / 16);
        bbfs_header->fat = stdio_drive_read (drive, 512 + 6 + n_sectors / 16, header_length_sectors * 512 - 6 - n_sectors / 16);

        bbfs_drives_list.n_drives += 1;
        if (bbfs_drives_list.n_drives == 1) {
            bbfs_drives_list.drives = kmalloc (0, sizeof (Bbfs_drive));
        } else {
            Bbfs_drive *old_drives = bbfs_drives_list.drives;
            bbfs_drives_list.drives = kmalloc (0, bbfs_drives_list.n_drives * sizeof (Bbfs_drive));

            for (u16 i = 0; i < (bbfs_drives_list.n_drives - 1) * sizeof (Bbfs_drive); i++) {
                bbfs_drives_list.drives[i] = old_drives[i];
            }

            kfree (old_drives);
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

void bbfs_sync (Bbfs_drive *drive) {
    stdio_drive_write (drive->id, 512 + 6, drive->n_sectors / 16, drive->header->free_mask);
    stdio_drive_write (drive->id, 512 + 6 + drive->n_sectors / 16, drive->n_sectors, drive->header->fat);
}