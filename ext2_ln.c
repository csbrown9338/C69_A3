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
        fprintf(stderr, "Usage: uhh it ain't right lmao");
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
    int dirinode = isValidDirectory(disk, truncatePath(dest));
    // Check if disk exists
    if (disk == NULL) {
        fprintf(stderr, "Invalid disk");
        exit(1);
    }
    // Check to make sure the source is a valid path
    if (srcinode == -1) {
        fprintf(stderr, "Invalid source file");
        return ENOENT;
    }
    // Check to make sure that the dest is also valid
    else if (dirinode == -1) {
        fprintf(stderr, "Invalid destination");
        return ENOENT;
    }
    // Check if name is taken
    else if (isValidPath(disk, dest) != -1) {
        fprintf(stderr, "Name is taken");
        if (isValidDirectory(disk, dest) != -1) return EISDIR;
        else if (isValidFile(disk, dest) != -1) return EEXIST;
        else exit(1);
    }
    // Check flag to see if it's a symbolic or hard link

    if (strcmp(flag, "-s")) {
        // she symbolic
        addSymLink(disk, extractFileName(dest), source, srcinode, dirinode);
    }
    else if (flag[0] == '\0') {
        // she hard
        addLinkFile(disk, extractFileName(dest), srcinode, dirinode);
    }
    else {
        fprintf(stderr, "Invalid flag");
        exit(1);
    }
    
    return 0;
}