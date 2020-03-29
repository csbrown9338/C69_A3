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
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "Usage: uhh it ain't right lmao");
        exit(1);
    }
    char *disk = argv[1];
    char *path = argv[2];
    char *flag;
    if (argc == 4) {
        flag = argv[3];
    }
    // If it's a file, then just... print da filename lmao
    if (isValidFile(path) != -1) printf("%s", path);
    // If it's a directory... oh boi
    else if (isValidPath(path) != -1) {
        // If there be the -a, do the . and ..
        if (strcmp(flag, "-a")) printf(".\n..");
        else if (flag[0] != '\0') {
            fprintf(stderr, "Invalid flag");
            exit(1);
        }
        // TODO: get the uhhhhh contents of the directory :)
    }
    // If not file or directory, then ENOENT
    else return ENOENT;
    return 0;
}