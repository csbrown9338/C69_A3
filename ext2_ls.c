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
        fprintf(stderr, "Usage: uhh it ain't right lmao");
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
        fprintf(stderr, "Invalid disk");
        exit(1);
    }
    inode = isValidDirectory(disk, path);
    // If it's a file, then just... print da filename lmao
    if (isValidFile(disk, path) != -1) printf("%s", path);
    // If it's a directory... oh boi
    else if (inode != -1) {
        // If there be the -a, do the . and ..
        int has_flag = 0; // 0 if no flag, 1 if -a flag
        if (strcmp(flag, "-a") == 0) has_flag = 1;
        else if (flag[0] != '\0') {
            fprintf(stderr, "Invalid flag");
            exit(1);
        }
        // TODO: get the uhhhhh contents of the directory :)
        // get the ext2_dir_entry of the current dir
        struct ext2_dir_entry *e = get_entry(disk, inode);
        int curr_pos = 0;
        while (curr_pos < 1024) {
            if (strcmp(e->name, ".") == 0 || strcmp(e->name, "..")) {
                if (has_flag == 1) printf("%s", e->name);
            }
            else printf("%s", e->name);
            curr_pos += e->rec_len;
        }
    }
    // If not file or directory, then ENOENT
    else return ENOENT;
    return 0;
}