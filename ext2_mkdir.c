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
        exit(1)
    }
    char *disk = argv[1];
    char *newdir = argv[2];
    // Check that everything before the last '/' is a valid dir
    if (isValidDirectory(newDir) == -1) { // TODO: TAKE OUT THE LAST '/'
        fprintf(stderr, "Invalid parent path");
        return ENOENT;
    }
    // Check if the name is taken
    else if (isValidPath(newDir) != -1) {
        fprintf(stderr, "Name is taken");
        if (isValidDirectory(newDir) != -1) return EISDIR;
        else if (isValidFile(newDir) != -1) return EEXIST;
        else exit(1);
    }
    else { 
        // make the new directory in inode stuff
    } 
    return 0;
}