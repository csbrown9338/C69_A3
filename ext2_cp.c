#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ext2.h"

int main(int argc, char **argv) {
    // Get the arguments
    if (argc != 4) {
        fprintf(stderr, "Usage: uhh it ain't right lmao");
        exit(1)
    }
    char *disk = argv[1];
    char *to_copy = argv[2];
    char *destdir = argv[3];
    // Check if the to_copy is a file (we're not doing dirs lol)

    // Check if the destdir is a valid directory in the disk
    
}