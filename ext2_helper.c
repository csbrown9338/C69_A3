#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "ext2.h"
#include "ext2_helper.h"

/*
 * takes out the last name in the path (get parent dir)
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
 * Gets the desired file name
 */
 char *extractFileName(char *path) {
    char *tpath = strtok(path, "/");
    char *name;
    while (tpath != NULL) {
        name = tpath;
        tpath = strtok(NULL, "/");
    }
    return name;
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

/*
 * Get the inode struct given the index
 */
struct ext2_inode *get_inode(unsigned char *disk, int inode) {
    return (struct ext2_inode *)((void *)disk + (sizeof(struct ext2_inode) * inode));
}

/*
 * Gets the dir entry given the inode and position
 */
struct ext2_dir_entry_2 *get_dir_entry(unsigned char *disk, struct ext2_inode *inode, int block, int pos) {
    int curr_block = inode->i_block[block];
    return struct ext2_dir_entry_2 *e = (struct ext2_dir_entry_2 *) (disk + (curr_block * EXT2_BLOCK_SIZE) + pos);
}

/*
 * Check if specified path is valid
 * returns -1 if invalid
 * returns inode number if valid
 */
int isValidPath(unsigned char *disk, char *path) {
    // Get the individual path names :)
    char *tpath = strtok(path, "/");
    int curr_inode = EXT2_ROOT_INO; // start at root
    while (tpath != NULL) {
        // Do the stuff to find the path :D
        // This is to parse through the block i think
        int curr_block = 0;
        int found_inode = curr_inode;
        struct ext2_inode *inode = get_inode(disk, curr_inode);
        // Loop through each block?
        while (found_inode != curr_inode && curr_block < inode->i_blocks) {
            int curr_pos = 0;
            // Loop through each position
            while (found _inode != curr_inode && curr_pos < inode->i_size) {
                // Go through all the entries in the directory to find a name match
                struct ext2_dir_entry_2 *e = get_dir_entry(disk, inode, curr_block, curr_pos);
                // check name if it MATCHES :D
                if (strcmp(tpath, e->name)) {
                    found_inode = e->inode;
                }
                curr_pos += e->rec_len; 
            }
            // If curr_pos is i_size, then we gotta go to the next block
            if (curr_pos == inode->i_size) curr_block++;
        }
        if (found_inode == curr_inode) return -1;
        tpath = strtok(NULL, "/");
    }
    return found_inode;
}

/*
 * Check if specified directory is valid
 * returns -1 if invalid
 * returns inode number if valid
 */
int isValidDirectory(unsigned char *disk, char *path) {
    int inode = isValidPath(disk, path);
    // Check if type is directory (EXT2_FT_DIR)
    struct ext2_dir_entry_2 *e = get_dir_entry(inode);
    if ((e->file_type == EXT2_FT_DIR)) return inode;
    return -1;
}

/*
 * Check if specified file is valid
 * returns -1 if invalid
 * returns inode number if valid
 */
int isValidFile(unsigned char *disk, char *path) {
    int inode = isValidPath(disk, path);
    // Check if type is file (EXT2_FT_REG_FILE)
    struct ext2_dir_entry_2 *e = get_dir_entry(inode);
    if ((e->file_type == EXT2_FT_REG_FILE)) return inode;
    return -1;
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
    struct ext2_dir_entry_2 *e = get_dir_entry(inode);
    if ((e->file_type == EXT2_FT_SYMLINK)) return inode;
    return -1;
}