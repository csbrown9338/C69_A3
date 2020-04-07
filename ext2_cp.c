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
    if (argc != 4) {
        fprintf(stderr, "Usage: <disk image file> <native file path> <absolute directory path>\n");
        exit(1);
    }
    char *disk_name = argv[1];
    char *to_copy = argv[2];
    char *destdir = argv[3];
    unsigned char *disk = readDisk(disk_name);
    int dir_inode = isValidDirectory(disk, destdir);
    // Check if disk exists
    if (disk == NULL) {
        fprintf(stderr, "Invalid disk\n");
        exit(1);
    }
    // Check if the destdir is a valid directory in the disk
    else if (dir_inode == -1) {
           fprintf(stderr, "Invalid destination\n"); 
           return ENOENT;
    }
    // Check if name is tooo long
    else if (strlen(extractFileName(to_copy)) >= EXT2_NAME_LEN) { 
        fprintf(stderr, "File name too long\n");
        return ENAMETOOLONG; 
    } 
    else {
        addNativeFile(disk, to_copy, dir_inode);
    }

    return 0;
}