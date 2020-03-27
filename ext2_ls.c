#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ext2.h"

int main(int argc, char **argv) {
    // Get the arguments
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "Usage: uhh it ain't right lmao");
        exit(1)
    }
    char *disk = argv[1];
    char *path = argv[2];
    char *flag;
    if (argc == 4) {
        flag = argv[3];
    }
    // CHeck if ya boi's a valid path

    // Check the type, i guess

    // If it's a file, then just... print da filename lmao

    // If it's a directory... oh boi

    // If there be the -a, do the . and ..

}