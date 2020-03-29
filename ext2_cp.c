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
        fprintf(stderr, "Usage: uhh it ain't right lmao");
        exit(1)
    }
    char *disk_name = argv[1];
    char *to_copy = argv[2];
    char *destdir = argv[3];
    unsigned char *disk = readDisk(disk_name);
    // Check if disk exists
    if (disk == NULL) {
        fprintf(stderr, "Invalid disk");
        exit(1);
    }
    // Check if the to_copy is a file (we're not doing dirs lol)
    if (isValidNativeFile(disk, to_copy) == -1) {
        fprintf(stderr, "Invalid source file");
        return ENOENT;
    }
    // Check if the destdir is a valid directory in the disk
    else if (isValidDirectory(disk, destdir) == -1) {
           fprintf(stderr, "Invalid destination"); 
           return ENOENT;
    }
    // Check if name is tooo long
    else if (1 == 0) { // TODO: find out how to see if a name is too long
        fprintf(stderr, "File name too long");
        return ENAMETOOLONG; 
    } 
    else {
        // TODO: c o p y (same file name)
    }

    return 0;
}