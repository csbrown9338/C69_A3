#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ext2.h"

int main(int argc, char **argv) {
    // Get arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: it wrong buddy boi");
        exit(1);
    }
    char *disk = argv[1];
    char *to_del = argv[2];
    // Check if to_del is a valid path

    // Uhhhhh delete it
}