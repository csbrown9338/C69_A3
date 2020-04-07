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



////////////////////////////////////////////
//  Functions for path stuff and disk :D  //
////////////////////////////////////////////



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

/*
 * Gets the desired file name
 */
 char *extractFileName(char *path) {
    int i = strlen(path);
    int index = -1;
    while (index == -1) {
        i--;
        if (path[i] == '/') {
            index = i;
        }
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



/////////////////////////////////////////////////////
//  Functions for getting basic info of the disk   //
/////////////////////////////////////////////////////



/*
 * returns the super block :D:D:D:D:D
 */
struct ext2_super_block *get_sb(unsigned char *disk) {
    return (struct ext2_super_block *)(disk + 1024);
}

/*
 * Gets the group description :)
 */
struct ext2_group_desc *get_gd(unsigned char *disk) {
    return (struct ext2_group_desc *)(disk + 2 * EXT2_BLOCK_SIZE);
}

/*
 * Gets the inode table :D
 */
struct ext2_inode *get_it(unsigned char *disk) {
    struct ext2_group_desc *gd = get_gd(disk);
    return (struct ext2_inode *) (disk + EXT2_BLOCK_SIZE * gd->bg_inode_table);
}

/*
 * Get the block bitmap of the disk
 */
unsigned char *get_b_bm(unsigned char *disk) {
    struct ext2_group_desc *gd = get_gd(disk);
    return (unsigned char *)(disk + EXT2_BLOCK_SIZE * gd->bg_block_bitmap);
}

/*
 * Get the inode bitmap of the disk
 */
unsigned char *get_i_bm(unsigned char *disk) {
    struct ext2_group_desc *gd = get_gd(disk);
    return (unsigned char *)(disk + EXT2_BLOCK_SIZE * gd->bg_inode_bitmap);
}

/*
 * Get the inode struct given the index
 */
struct ext2_inode *get_inode(unsigned char *disk, int inode) {
    struct ext2_inode *i = get_it(disk);
    return (struct ext2_inode *)(i + (inode - 1));
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
    // struct ext2_inode *i = &get_it(disk)[inode];
    return (struct ext2_dir_entry_2 *)(disk + EXT2_BLOCK_SIZE * (i->i_block)[0]);
}

/*
 * Gets the block
 */
unsigned char *get_block(unsigned char *disk, struct ext2_inode *i, int block) {
    return (unsigned char *) (disk + (i->i_block[block] * EXT2_BLOCK_SIZE));
}

/*
 * checks state of bit
 */
int bit_in_use(unsigned char byte, int offset) {
    // return (byte & (1 << offset));
    return (byte >> offset) & 1;
}



//////////////////////////////////////////////////////////////
//  Functions to check the validity of a path in the disk   //
//////////////////////////////////////////////////////////////



/*
 * Check if specified path is valid
 * returns -1 if invalid
 * returns inode number if valid
 */
int isValidPath(unsigned char *disk, char *og_path) {
    char path[strlen(og_path)];
    // Make copy of path, cuz strtok apparently adds dumb nullbytes in between smhhhh
    strcpy(path, og_path);
    // Check if it's ROOOOOT (/)
    if (strcmp(path, "/") == 0) return EXT2_ROOT_INO;
    // Get the individual path names :)
    char *tpath = strtok(path, "/");
    int curr_inode = EXT2_ROOT_INO; // start at root
    int found_inode;
    int found_file = 0;
    // Starting the loop to go through each token in the path
    while (tpath != NULL && found_file == 0) {
        // Do the stuff to find the path :D
        int curr_block = 0;
        struct ext2_inode *inode = get_inode(disk, curr_inode);
        found_inode = curr_inode;
        // Loop through each block
        while (found_inode == curr_inode && curr_block < inode->i_blocks && found_file == 0) {
            int curr_pos = 0;
            // Loop through each position
            while (found_inode == curr_inode && curr_pos < inode->i_size && found_file == 0) {
                // Go through all the entries in the directory to find a name match
                struct ext2_dir_entry_2 *e = get_dir_entry(disk, inode, curr_block, curr_pos);
                // check name if it MATCHES :D
                if (strncmp(tpath, e->name, strlen(tpath)) == 0) {
                    curr_inode = e->inode;
                }
                if (e->file_type != EXT2_FT_DIR) found_file = 1;
                curr_pos += e->rec_len; 
            }
            // If curr_pos is i_size, then we gotta go to the next block
            if (found_inode == curr_inode) curr_block++;
        }
        if (found_inode == curr_inode && found_file == 0) return -1;
        // +1 for null byte
        tpath = strtok(NULL, "/");
    }
    // I think another if statement takes care of this but... just to be safe lol
    if (found_file == 1 && tpath != NULL) return -1;
    return curr_inode;
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
    if (e->file_type == EXT2_FT_DIR) return inode;
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
    struct ext2_inode *i = get_inode(disk, inode);
    if ((i->i_mode & EXT2_S_IFREG) || (i->i_mode & EXT2_S_IFLNK)) return inode;
    return -1;
}



/////////////////////////
//  Misc helpers lol   //
/////////////////////////



/*
 * returns the first free block given the size
 * and allocates the rest of the blocks :)
 * size says how many blocks are needed
 */
int allocateBlocks(unsigned char *disk, struct ext2_inode *i, int size) {
    // Get block bitmap
    unsigned char *bbm = get_b_bm(disk);
    // also super block
    struct ext2_super_block *sb = get_sb(disk);
    // Assign amount of blocks in inode to be 0
    i->i_blocks = 0;
    // Loop through the bitmaperino
    // One loop for every byte, another for every bit, and another for size
    int bit = 0;
    int curr_size = 0;
    int byte = 0;
    int offset = 0;
    int curr_block = 0;
    int first = -1;
    while (bit < sb->s_blocks_count && curr_size != size) {
        byte = bit / (sizeof(unsigned char) * 8);
        offset = bit % (sizeof(unsigned char) * 8);
        // If bit is free
        if (bit_in_use(bbm[byte], offset) == 0) {
            // Check if this is the first :)
            if (first == -1) first = bit;
            // point to dis block from inode :)))
            i->i_block[curr_block] = bit + 1;
            curr_block++;
            // also make bit in use and change counters
            bbm[byte] |= 1 << offset;
            i->i_blocks++;
            sb->s_free_blocks_count--;
            curr_size++;
        }
        bit++;
    }
    if (curr_size != size) {
        fprintf(stderr, "couldn't find blocks, sorry\n");
        // deallocate everything :(((
        return -1;
    }
    return first;
}

/*
 * Returns the node that was allocated
 * size is required amount of blocks
 * if there aren't enough blocks, uh gg returns -1
 * used for creation of new files and directories?
 */
int allocateInode(unsigned char *disk, int size) {
    // Get the amount of free blocks we have
    if (size > (get_gd(disk))->bg_free_blocks_count) {
        fprintf(stderr, "Not enough space to allocate inode\n");
        return ENOMEM;
    }
    // loop through the inode bitmap to see what's free!!!!
    unsigned char *ibm = get_i_bm(disk);
    int curr_block = 0;
    // Look for a free inodeeee
    struct ext2_super_block *sb = get_sb(disk);
    while (curr_block < sb->s_inodes_count/(sizeof(unsigned char) * 8)) {
        int bit = 0;
        while (bit < 8) {
            if (bit_in_use(ibm[curr_block], bit) == 0) {
                int found_inode = (curr_block * 8) + bit + 1;
                fflush(stdout);
                ibm[curr_block] |= 1 << bit; // set it to in use
                sb->s_free_inodes_count--;
                return found_inode;
            }
            bit++;
        }
        curr_block++;
    }
    fprintf(stderr, "Couldn't find an inode :(\n");
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
    struct ext2_super_block *sb = get_sb(disk);
    int used_inodes = sb->s_inodes_count - sb->s_free_inodes_count;
    // loooooopp through the blocks :)
    int curr_block = 0;
    while (curr_block < dir->i_blocks) {
        // unsigned int block = dir->i_block[curr_block];
        int curr_pos = 0;
        while (curr_pos < dir->i_size + 1) {
            struct ext2_dir_entry_2 *e = get_dir_entry(disk, dir, curr_block, curr_pos);
            if (e->inode <= 0 || e->inode > used_inodes) return e;
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
  * Copies the contents of the file!!!
  * returns 0 on success, -1 on failure
  */
int addFileContents(unsigned char *disk, struct ext2_inode *i, int fd) {
    int curr_block = 0;
    int total_size = 0;
    unsigned char *block = get_block(disk, i, curr_block);
    int size_read = 0;
    // direct blocksssss
    while (curr_block < 12 && (size_read = read(fd, block, EXT2_BLOCK_SIZE)) > 0) {
        // increase inode size, and update counters
        i->i_size += size_read;
        total_size += size_read;
        curr_block++;
    }
    if (size_read == 0) return 0;
    // indirect blockssssss
    return -1;
}



/////////////////////////////////////////////
// The functions for the ACTUAL COMMANDS   //
/////////////////////////////////////////////



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
    // get the inode
    struct ext2_inode *in = get_inode(disk, allocatedinode);
    // get first free block and allocate all nodes
    allocateBlocks(disk, in, blocks);
    // Copy the file infoooooo
    addFileContents(disk, in, fd);
    // Change mode
    in->i_mode |= EXT2_S_IFREG;
    // Find an empty spot in the parent directory
    struct ext2_dir_entry_2 *entry = findNewEntry(disk, inode);
    if (entry == NULL) {
        fprintf(stderr, "Couldn't find a free space in the directory\n");
        return -1;
    }
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
    if (allocatedinode == -1) return -1;
    // put er innnnn
    struct ext2_inode *in = get_inode(disk, allocatedinode);
    // Find blocks
    allocateBlocks(disk, in, 1);
    struct ext2_dir_entry_2 *a_entry = get_dir_entry(disk, in, 0, 0);
    addEntry(a_entry, allocatedinode, EXT2_FT_DIR, ".");
    // Update size of new dir :)
    in->i_size = a_entry->rec_len;
    // Add another entry for parent
    struct ext2_dir_entry_2 *parent = findNewEntry(disk, allocatedinode);
    addEntry(parent, inode, EXT2_FT_DIR, "..");
    // Add this too
    in->i_size += parent->rec_len;
    // the modeeeeeee 
    in->i_mode |= EXT2_S_IFDIR;
    // now add it to the parent dirrrr
    struct ext2_dir_entry_2 *entry = findNewEntry(disk, inode);
    if (entry == NULL) {
        fprintf(stderr, "Couldn't find a free space in the directory\n");
        return -1;
    }
    // and add
    addEntry(entry, allocatedinode, EXT2_FT_DIR, dirname);
    // Update links
    in->i_links_count = 2;
    struct ext2_inode *pinode = get_inode(disk, inode);
    pinode->i_links_count++;
    pinode->i_size += entry->rec_len;
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
    // Get inode
    struct ext2_inode *in = get_inode(disk, allocatedinode);
    // Find blockssss
    allocateBlocks(disk, in, blocks);
    // memcpy the path
    unsigned char *block = get_block(disk, in, 0);
    memcpy(block, source_name, sizeof(source_name));
    // mode
    in->i_mode |= EXT2_S_IFLNK;
    // put it into the parent dirrrrrrrrr
    struct ext2_dir_entry_2 *entry = findNewEntry(disk, dir_inode);
    if (entry == NULL) {
        fprintf(stderr, "Couldn't find a free space in the directory\n");
        return -1;
    }
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
    if (entry == NULL) {
        fprintf(stderr, "Couldn't find a free space in the directory\n");
        return -1;
    }
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