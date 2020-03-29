#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "ext2.h"

/*
 * Check if specified path is valid
 * returns -1 if invalid
 * returns inode number if valid
 */
int isValidPath(char *path) {
    return 0;
}

/*
 * Check if specified directory is valid
 * returns -1 if invalid
 * returns inode number if valid
 */
int isValidDirectory(char *path) {
    return 0;
}

/*
 * Check if specified file is valid
 * returns -1 if invalid
 * returns inode number if valid
 */
int isValidFile(char *path) {
    return 0;
}

/*
 * Check if specified file is valid in native
 * returns -1 if invalid
 * returns inode number if valid
 */
int isValidNativeFile(char *path) {
    return 0;
}

/*
 * Check if specified link is valid
 * returns -1 if invalid
 * returns inode number if valid
 */
int isValidLink(char *path) {
    return 0;
}


unsigned char *readDisk(char *path) {
    return NULL;
}