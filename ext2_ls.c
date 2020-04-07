#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "ext2.h"
#include "ext2_helper.h"

int main(int argc, char **argv) {
    // Get the arguments
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "Usage: <disk image file> <file absolute path> [-a]\n");
        exit(1);
    }
    char *disk_name = argv[1];
    char *path = argv[2];
    char *flag;
    if (argc == 4) {
        flag = argv[3];
    }
    unsigned char *disk = readDisk(disk_name);
    int inode;
    // Check if disk exists
    if (disk == NULL) {
        fprintf(stderr, "Invalid disk\n");
        exit(1);
    }
    inode = isValidDirectory(disk, path);
    int fileInode = isValidFile(disk, path);
    printf("inode received: %d\n", inode);
    if (inode != -1) {
        // If there be the -a, do the . and ..
        int has_flag = 0; // 0 if no flag, 1 if -a flag
        if (strcmp(flag, "-a") == 0) has_flag = 1;
        // get the ext2_dir_entry of the current dir
        struct ext2_inode *i = get_inode(disk, inode);
        int curr_block = 0;
        while (curr_block < i->i_blocks){
            int curr_pos = 0;
            int linkidx = 0;
            // get the amount of LINKS oh my goodness this is so important
            int links = i->i_links_count;
            if (inode != EXT2_ROOT_INO) links++;
            while (curr_pos < i->i_size && linkidx < links) {
                struct ext2_dir_entry_2 *e = get_dir_entry(disk, i, curr_block, curr_pos);
                // printf("namelength: %d, ", e->name_len);
                if (strncmp(e->name, ".", 1) == 0 || strncmp(e->name, "..", 2) == 0) {
                    if (has_flag == 1) printf("%s\n", e->name);
                }
                else if (e->name_len != 0) printf("%s\n", e->name);
                curr_pos += e->rec_len;
                linkidx++;
            }
            curr_block++;
        }
    }
    // If it's a file, then just... print da filename lmao
    if (fileInode != -1) printf("%s\n", extractFileName(path));
    // If not file or directory, then ENOENT
    else return ENOENT;
    return 0;
}