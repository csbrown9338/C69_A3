#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ext2.h"

int main(int argc, char **argv) {
    // Get the arguments
    if (argc != 4 && argc != 5) {
        fprintf(stderr, "Usage: uhh it ain't right lmao");
        exit(1)
    }
    char *disk = argv[1];
    char *source = argv[2];
    char *dest = argv[3];
    char *flag;
    if (argc == 5) {
        flag = argv[4];
    }
    // Check to make sure the source is a valid path

    // Check to make sure that the dest is also valid

    // Check flag to see if it's a symbolic or hard link
    
}