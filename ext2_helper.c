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
        if (path[i] == '/') {
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
    return (struct ext2_group_desc *) ((void *)disk + (2 * EXT2_BLOCK_SIZE));
}

/*
 * Get the inode struct given the index
 */
struct ext2_inode *get_inode(unsigned char *disk, int inode) {
    struct ext2_group_desc *gd = get_gd(disk);
    return (struct ext2_inode *)((void *)gd->bg_inode_table + (sizeof(struct ext2_inode) * inode));
}

/*
 * Gets the dir entry given the inode, block, and position
 * (meant to be used mostly in the isValidPath helper)
 */
struct ext2_dir_entry_2 *get_dir_entry(unsigned char *disk, struct ext2_inode *inode, int block, int pos) {
    int curr_block = inode->i_block[block];
    return (struct ext2_dir_entry_2 *)(disk + (curr_block * EXT2_BLOCK_SIZE) + pos);
}

/*
 * get dir entry given only the inode index
 */
struct ext2_dir_entry_2 *get_entry(unsigned char *disk, int inode) {
    struct ext2_inode *i = get_inode(disk, inode);
    return (struct ext2_dir_entry_2 *)(i->i_block[0]);
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
    int found_inode = curr_inode;
    while (tpath != NULL) {
        // Do the stuff to find the path :D
        // This is to parse through the block i think
        int curr_block = 0;
        struct ext2_inode *inode = get_inode(disk, curr_inode);
        // Loop through each block?
        while (found_inode != curr_inode && curr_block < inode->i_blocks) {
            int curr_pos = 0;
            // Loop through each position
            while (found_inode != curr_inode && curr_pos < inode->i_size) {
                // Go through all the entries in the directory to find a name match
                struct ext2_dir_entry_2 *e = get_dir_entry(disk, inode, curr_block, curr_pos);
                // check name if it MATCHES :D
                if (strcmp(tpath, e->name)) {
                    if (e->file_type != EXT2_FT_DIR) return -1;
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
    struct ext2_dir_entry_2 *e = get_entry(disk, inode);
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
    struct ext2_dir_entry_2 *e = get_entry(disk, inode);
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
    struct ext2_dir_entry_2 *e = get_entry(disk, inode);
    if ((e->file_type == EXT2_FT_SYMLINK)) return inode;
    return -1;
}

/*
 * Adds a file into the directory given by the inode
 * path is the native path to the file
 * returns 0 on success, and -1 on failure
 */
int addNativeFile(unsigned char *disk, char *path, int inode) {
    char *fname = extractFileName(path);
    return 0;
}

/*
 * Adds a directory into the directory given by the inode
 * returns 0 on success, and -1 on failure
 */
int addDir(unsigned char *disk, char *dirname, int inode) {
    return 0;
}

/*
 * Adds a sym link into the directory given by the inode
 * returns 0 on success, and -1 on failure
 */
int addLink(unsigned char *disk, char *lname, int file_inode, int dir_inode) {
    return 0;
}

/*
 * Adds a hard link into the directory given by the inode
 * returns 0 on success, and -1 on failure
 */
int addLinkFile(unsigned char *disk, char *lname, int link_inode, int dir_inode) {
    return 0;
}

/*
 * Deletes a file or link given the inode to_del
 * returns 0 on success, and -1 on failure
 */
int delFile(unsigned char *disk, int to_del_inode, int parent_inode) {
    return 0;
}