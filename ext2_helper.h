#include <string.h>
#include "ext2.h"

char *truncatePath(char *path);

char *extractFileName(char *path);

unsigned char *readDisk(char *path);

struct ext2_group_desc *get_gd(unsigned char *disk);

struct ext2_inode *get_inode(unsigned char *disk, int inode);

struct ext2_dir_entry_2 *get_dir_entry(unsigned char *disk, struct ext2_inode *inode, int block, int pos);

struct ext2_dir_entry_2 *get_entry(unsigned char *disk, int inode)

int isValidPath(unsigned char *disk, char *path);

int isValidDirectory(unsigned char *disk, char *path);

int isValidFile(unsigned char *disk, char *path);

int isValidNativeFile(unsigned char *disk, char *path);

int isValidLink(unsigned char *disk, char *path);