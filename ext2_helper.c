#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "ext2.h"

/*
 * takes out the last name in the path 
 */
char *truncatePath(char *path) {
    int i = strlen(path) - 1;
    int index = -1;
    while (index == -1) {
        if (strcmp(path[i], "/")) {
            path[i] = '\0';
            index = i;
        }
        i--;
    }
    return path;
}

/*
 * Check if specified path is valid
 * returns -1 if invalid
 * returns inode number if valid
 */
int isValidPath(unsigned char *disk, char *path) {
    return 0;
}

/*
 * Check if specified directory is valid
 * returns -1 if invalid
 * returns inode number if valid
 */
int isValidDirectory(unsigned char *disk, char *path) {
    int inode = isValidPath(disk, path);
    // Check if type is directory (EXT2_FT_DIR)
    return 0;
}

/*
 * Check if specified file is valid
 * returns -1 if invalid
 * returns inode number if valid
 */
int isValidFile(unsigned char *disk, char *path) {
    int inode = isValidPath(disk, path);
    // Check if type is file (EXT2_FT_REG_FILE)
    return 0;
}

/*
 * Check if specified file is valid in native
 * returns -1 if invalid
 * returns inode number if valid
 */
int isValidNativeFile(unsigned char *disk, char *path) {
    return 0;
}

/*
 * Check if specified link is valid
 * returns -1 if invalid
 * returns inode number if valid
 */
int isValidLink(unsigned char *disk, char *path) {
    int inode = isValidPath(disk, path);
    // Check if type is symlink (EXT2_FT_SYMLINK)
    return 0;
}

/*
 * Reads the disk given a path to the disk img
 */
unsigned char *readDisk(char *path) {
    // Open da file
    int fd = open(path, O_RDWR);
    // the disk!!!! use mmap apparently
    unsigned char *d = mmap(NULL, EXT2_BLOCK_SIZE * 128, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    // But we gotta make sure that it actually worked
    if (d == MAP_FAILED) {
        fprintf(stderr, "Mapping didn't work");
        exit(1);
    }
    return d;
}

/*
 * Gets the group description :)
 */
struct ext2_group_desc *get_gd(unsigned char *disk) {
    return (struct ext2_group_desc *) (void *)disk + (2 * EXT2_BLOCK_SIZE);
}

// struct ext2_inode *get_it(unsigned char *disk) {
//     struct ext2_group_desc *gd = get_gd(disk);
//     return (struct ext2_inode *) 
// }

int get_bg(unsigned char *disk, int inode) {
    return (inode - 1) / (((struct ext2_super_block *)disk)->s_inodes_per_group);
}