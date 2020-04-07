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
    if (argc != 4 && argc != 5) {
        fprintf(stderr, "Usage: <disk image file> <source path> <destination path> [-s]\n");
        exit(1);
    }
    char *disk_name = argv[1];
    char *source = argv[2];
    char *dest = argv[3];
    char *flag;
    if (argc == 5) {
        flag = argv[4];
    }
    unsigned char *disk = readDisk(disk_name);
    int srcinode = isValidFile(disk, source);
    // int exists_inode = isValidPath(disk, dest);
    char *raw_name = extractFileName(dest);
    char filename[strlen(raw_name) + 1];
    strncpy(filename, raw_name, strlen(raw_name));
    filename[strlen(raw_name)] = '\0';
    // printf("filename: %s, length: %d\n", filename, strlen(filename));
    char *parent = truncatePath(dest);
    int dirinode = isValidDirectory(disk, parent);
    // Check if disk exists
    if (disk == NULL) {
        fprintf(stderr, "Invalid disk\n");
        exit(1);
    }
    // Check to make sure the source is a valid path
    if (srcinode == -1) {
        fprintf(stderr, "Invalid source file\n");
        return ENOENT;
    }
    // Check to make sure that the dest is also valid
    else if (dirinode == -1) {
        fprintf(stderr, "Invalid destination\n");
        return ENOENT;
    }
    // Check if name is taken
    // else if (exists_inode != -1) {
    //     fprintf(stderr, "Name is taken");
    //     if (isValidDirectory(disk, dest) != -1) return EISDIR;
    //     else if (isValidFile(disk, dest) != -1) return EEXIST;
    //     else exit(1);
    // }
    // // Check flag to see if it's a symbolic or hard link

    if (strncmp(flag, "-s", 2) == 0) {
        // she symbolic
        int ret = addSymLink(disk, filename, source, srcinode, dirinode);
        fprintf(stderr, "Couldn't add symbolic link\n");
    }
    else if (flag[0] == '\0') {
        // she hard
        int ret = addLinkFile(disk, filename, srcinode, dirinode);
        fprintf(stderr, "Couldn't add hard link\n");
    }
    else {
        fprintf(stderr, "Invalid flag\n");
        exit(1);
    }
    
    return 0;
}