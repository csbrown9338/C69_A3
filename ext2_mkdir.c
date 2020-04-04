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
        fprintf(stderr, "Usage: uhh it ain't right lmao");
        exit(1);
    }
    char *disk_name = argv[1];
    char *newdir = argv[2];
    unsigned char *disk = readDisk(disk_name);
    int exists_inode = isValidPath(disk, newdir);
    char *raw_name = extractFileName(newdir);
    char *filename;
    strncpy(filename, raw_name, strlen(raw_name));
    printf("name received: %s\n", filename);
    char *parent = truncatePath(newdir);
    printf("name after truncate: %s\n", filename);
    int inode = isValidDirectory(disk, parent);
    // Check if disk exists
    if (disk == NULL) {
        fprintf(stderr, "Invalid disk");
        exit(1);
    }
    // Check that everything before the last '/' is a valid dir
    if (inode == -1) {
        fprintf(stderr, "Invalid parent path");
        return ENOENT;
    }
    // Check if the name is taken
    else if (exists_inode != -1) {
        fprintf(stderr, "Name is taken");
        if (isValidDirectory(disk, newdir) != -1) return EISDIR;
        else if (isValidFile(disk, newdir) != -1) return EEXIST;
        else exit(1);
    }
    else { 
        addDir(disk, filename, inode);
    } 
    return 0;
}