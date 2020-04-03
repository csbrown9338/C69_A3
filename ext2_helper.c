#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <math.h>
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
            index = i;
            path[i] = '\0';
        }
        i--;
    }
    return path;
}

// /*
//  * returns tokens of path
//  */
//  char **tokenizePath(char *path) {
//     // loop through the path
//     int i = 0;
//     int tokenindex = 0;
//     char **tpath;
//     while (i < strlen(path)) {
//         if (path[i] == '/') {
//             if (i != 0) {
//                 **(tpath + sizeof(char) * i) = '\0';
//                 tokenindex++;
//             }
//         }
//         else **(tpath + sizeof(char) * i) = path[i];
//         printf("aklsdjhalkdjfh\n");
//         fflush(stdout);
//         i++;
//     }
//     return tpath;
//  }

/*
 * Gets the desired file name
 */
 char *extractFileName(char *path) {
    int i = strlen(path) - 1;
    int index = -1;
    while (index == -1) {
        if (path[i] == '/') {
            index = i;
        }
        i--;
    }
    char *name = &(path[i + 1]);
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
    close(fd);
    // But we gotta make sure that it actually worked
    if (d == MAP_FAILED) {
        fprintf(stderr, "Mapping didn't work");
        exit(1);
    }
    return d;
}

/*
 * returns the super block :D:D:D:D:D
 */
struct ext2_super_block *get_sb(unsigned char* disk) {
    return (struct ext2_super_block *)(disk + 1024);
}

/*
 * Gets the group description :)
 */
struct ext2_group_desc *get_gd(unsigned char *disk) {
    return (struct ext2_group_desc *)(disk + 2 * EXT2_BLOCK_SIZE);
}

struct ext2_inode *get_it(unsigned char *disk) {
    struct ext2_group_desc *gd = get_gd(disk);
    return (struct ext2_inode *) (disk + EXT2_BLOCK_SIZE * gd->bg_inode_table);
}

/*
 * Get the block bitmap of the disk
 */
unsigned int get_b_bm(unsigned char *disk) {
    return (get_gd(disk))->bg_block_bitmap;
}

/*
 * Get the inode bitmap of the disk
 */
unsigned int get_i_bm(unsigned char *disk) {
    return (get_gd(disk))->bg_inode_bitmap;
}

/*
 * Get the inode struct given the index
 */
struct ext2_inode *get_inode(unsigned char *disk, int inode) {
    struct ext2_inode *i = get_it(disk);
    return (struct ext2_inode *)(i + (inode-1));
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
    return (struct ext2_dir_entry_2 *)(disk + EXT2_BLOCK_SIZE * (i->i_block)[0]);
}

/*
 * Check if specified path is valid
 * returns -1 if invalid
 * returns inode number if valid
 */
int isValidPath(unsigned char *disk, char *og_path) {
    char path[strlen(og_path)];
    strcpy(path, og_path);
    printf("passed in path: %s\n", path);
    // Make copy of path, cuz strtok apparently adds dumb nullbytes in between smhhhh
    // Check if it's ROOOOOT (/)
    if (strcmp(path, "/") == 0) return EXT2_ROOT_INO;
    // Get the individual path names :)
    char *tpath = strtok(path, "/");
    printf("tokenize successful\n");
    fflush(stdout);
    int curr_inode = EXT2_ROOT_INO; // start at root
    int found_inode = curr_inode;
    // Starting the loop to go through each token in the path
    while (tpath != '\0') {
        // Do the stuff to find the path :D
        printf("looking for: %s\n", tpath);
        int curr_block = 0;
        struct ext2_inode *inode = get_inode(disk, curr_inode);
        // Loop through each block
        while (found_inode == curr_inode && curr_block < inode->i_blocks) {
            int curr_pos = 0;
            // Loop through each position
            while (found_inode == curr_inode && curr_pos < inode->i_size) {
                // Go through all the entries in the directory to find a name match
                struct ext2_dir_entry_2 *e = get_dir_entry(disk, inode, curr_block, curr_pos);
                // check name if it MATCHES :D
                if (strcmp(tpath, e->name) == 0) {
                    found_inode = e->inode;
                }
                curr_pos += e->rec_len; 
            }
            // If curr_pos is i_size, then we gotta go to the next block
            curr_block++;
        }
        if (found_inode == curr_inode) return -1;
        // +1 for null byte
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
 * Returns the node that was allocated
 * size is required amount of blocks
 * if there aren't enough blocks, uh gg returns -1
 * used for creation of new files and directories?
 */
int allocateInode(unsigned char *disk, int size) {
    // Get the amount of free blocks we have
    if (size < (get_gd(disk))->bg_free_blocks_count) {
        fprintf(stderr, "Not enough space to allocate inode");
        return ENOMEM;
    }
    // loop through the inode bitmap to see what's free!!!!
    unsigned char *ibm = (void *)get_i_bm(disk);
    int curr_inode = EXT2_ROOT_INO;
    // Look for a free inodeeee
    while (curr_inode < (get_sb(disk))->s_inodes_count) {
        if (ibm[curr_inode] == 0) {
            ibm[curr_inode] |= 1; // set it to in use
            return curr_inode;
        }
        curr_inode++;
    }
    fprintf(stderr, "Couldn't find an inode :(");
    return -1; // couldn't find one gg idk what to tell you
}

/*
 * Searches for next new space in inode
 * Returns unused ext2_dir_new_entry_2 structure, otherwise NULL
 * not neccesarily for new entries, but also for hard links
 */
 struct ext2_dir_entry_2 *findNewEntry(unsigned char *disk, int dir_inode) {
    // Get the dir_inode
    struct ext2_inode *dir = get_inode(disk, dir_inode);
    // loooooopp through the blocks :)
    int curr_block = 0;
    while (curr_block < dir->i_blocks) {
        unsigned int block = dir->i_block[curr_block];
        int curr_pos = block;
        while (curr_pos < EXT2_BLOCK_SIZE) {
            struct ext2_dir_entry_2 *e = (struct ext2_dir_entry_2 *) curr_pos;
            if (e->inode == 0) return e;
            curr_pos += e->rec_len;
        }
        curr_block++;
    }
    return NULL;
 }

/*
 * Adds info to the dir entry :)
 * returns 0 on success!
 */
 int addEntry(struct ext2_dir_entry_2 *entry, int inode, int file_type, char *name) {
    entry->inode = inode;
    entry->name_len = strlen(name);
    entry->rec_len = ceil((sizeof(struct ext2_dir_entry_2) + sizeof(name)) / 4) * 4;
    entry->file_type = file_type;
    strcpy(entry->name, name);
    return 0;
 }

/*
 * Adds a file into the directory given by the inode
 * path is the native path to the file
 * returns 0 on success, and -1 on failure
 */
int addNativeFile(unsigned char *disk, char *path, int inode) {
    char *fname = extractFileName(path);
    // Get the file handle
    int fd = open(path, O_RDONLY);
    // get the amount of blocks you need to allocate
    int blocks = lseek(fd, 0, SEEK_END) / EXT2_BLOCK_SIZE;
    // Find an inode that you can allocate to
    int allocatedinode = allocateInode(disk, blocks);
    // Put it in
    struct ext2_dir_entry_2 *a_entry = (struct ext2_dir_entry_2 *)(get_inode(disk, allocatedinode))->i_block[0];
    addEntry(a_entry, allocatedinode, EXT2_FT_REG_FILE, fname);
    // Find an empty spot in the parent directory
    struct ext2_dir_entry_2 *entry = findNewEntry(disk, inode);
    // Do the same thaaaaaannnnnng but with the parent dirrrr
    addEntry(entry, allocatedinode, EXT2_FT_REG_FILE, fname);
    // close file handle btw
    close(fd);
    return 0;
}

/*
 * Adds a directory into the directory given by the inode
 * returns 0 on success, and -1 on failure
 */
int addDir(unsigned char *disk, char *dirname, int inode) {
    // get a freeeeeeeeeee inode :D:D:D
    int allocatedinode = allocateInode(disk, 1);
    // put er innnnn
    struct ext2_dir_entry_2 *a_entry = (struct ext2_dir_entry_2 *)(get_inode(disk, allocatedinode))->i_block[0];
    addEntry(a_entry, allocatedinode, EXT2_FT_DIR, dirname);
    // now add it to the parent dirrrr
    struct ext2_dir_entry_2 *entry = findNewEntry(disk, inode);
    // and add
    addEntry(entry, allocatedinode, EXT2_FT_DIR, dirname);
    return 0;
}

/*
 * Adds a sym link into the directory given by the inode
 * returns 0 on success, and -1 on failure
 */
int addSymLink(unsigned char *disk, char *lname, char *source_name, int file_inode, int dir_inode) {
    // Get the amount of blocks ya neeeed
    int blocks = ceil(sizeof(source_name) / EXT2_BLOCK_SIZE);
    // do the do
    int allocatedinode = allocateInode(disk, blocks);
    // put em INNNN BOII
    struct ext2_dir_entry_2 *a_entry = (struct ext2_dir_entry_2 *)(get_inode(disk, allocatedinode))->i_block[0];
    addEntry(a_entry, allocatedinode, EXT2_FT_SYMLINK, lname);
    // put it into the parent dirrrrrrrrr
    struct ext2_dir_entry_2 *entry = findNewEntry(disk, dir_inode);
    // and addddddd
    addEntry(entry, allocatedinode, EXT2_FT_SYMLINK, lname);
    return 0;
}

/*
 * Adds a hard link into the directory given by the inode
 * returns 0 on success, and -1 on failure
 */
int addLinkFile(unsigned char *disk, char *lname, int link_inode, int dir_inode) {
    // don't need to allocate a new inode, just add it into the inode of the directory
    struct ext2_dir_entry_2 *entry = findNewEntry(disk, dir_inode);
    // add
    addEntry(entry, link_inode, EXT2_FT_REG_FILE, lname);
    return 0;
}

/*
 * Deletes a file or link given the inode to_del
 * returns 0 on success, and -1 on failure
 */
int delFile(unsigned char *disk, int to_del_inode, int parent_inode) {
    // get the inode and make it 0, and remove the uh yeah
    return 0;
}