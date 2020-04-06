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
    // Get args
    if (argc != 3) {
        fprintf(stderr, "Usage: <disk image file> <new directory path>");
        exit(1);
    }
    char *disk_name = argv[1];
    char *newdir = argv[2];
    unsigned char *disk = readDisk(disk_name);
    // Check if disk exists
    if (disk == NULL) {
        fprintf(stderr, "Invalid disk");
        exit(1);
    }
    // If the disk exists :)
    // printf("full path: %s, length: %d\n", newdir, strlen(newdir));
    int exists_inode = isValidPath(disk, newdir);
    int exists_inode = isValidPath(disk, dest);
    char *raw_name = extractFileName(newdir);
    // printf("raw name: %s, length: %d\n", raw_name, strlen(raw_name));
    char filename[strlen(raw_name)];
    strncpy(filename, raw_name, strlen(raw_name));
    char *parent = truncatePath(newdir);
    int inode = isValidDirectory(disk, parent);
    // Check that everything before the last '/' is a valid dir
    if (inode == -1) {
        fprintf(stderr, "Invalid parent path");
        memset(filename, '\0', strlen(filename));
        return ENOENT;
    }
    
    else if (exists_inode == -1) { 
        printf("we passed all checks\n");
        fflush(stdout);
        addDir(disk, filename, inode);
        memset(filename, '\0', strlen(filename));
    }
    // Check if the name is taken
    else {
        fprintf(stderr, "Name is taken");
        memset(filename, '\0', strlen(filename));
        if (isValidDirectory(disk, newdir) != -1) return EISDIR;
        else if (isValidFile(disk, newdir) != -1) return EEXIST;
        else exit(1);
    }
    return 0;
}