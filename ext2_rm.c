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
    // Get arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: it wrong buddy boi");
        exit(1);
    }
    char *disk_name = argv[1];
    char *to_del = argv[2];
    unsigned char *disk = readDisk(disk_name);
    // Check if disk exists
    if (disk == NULL) {
        fprintf(stderr, "Invalid disk");
        exit(1);
    }
    // Check if to_del is a valid path
    if (isValidPath(disk, to_del) == -1) {
        fprintf(stderr, "Not a valid path");
        return ENOENT;
    }
    else if (isValidDirectory(disk, to_del) != -1) {
        fprintf(stderr, "Not deleting directories");
        return EISDIR;
    }
    else {
        // Uhhhhh delete it
    }
    

    return 0;
}