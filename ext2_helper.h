#include <string.h>
#include "ext2.h"

char *truncatePath(char *path);

char *extractFileName(char *path);

unsigned char *readDisk(char *path);

struct ext2_group_desc *get_gd(unsigned char *disk);

unsigned int get_b_bm(unsigned char *disk);

unsigned int get_i_bm(unsigned char *disk)

struct ext2_inode *get_inode(unsigned char *disk, int inode);

struct ext2_dir_entry_2 *get_dir_entry(unsigned char *disk, struct ext2_inode *inode, int block, int pos);

struct ext2_dir_entry_2 *get_entry(unsigned char *disk, int inode);

int isValidPath(unsigned char *disk, char *path);

int isValidDirectory(unsigned char *disk, char *path);

int isValidFile(unsigned char *disk, char *path);

int isValidLink(unsigned char *disk, char *path);

int addNativeFile(unsigned char *disk, char *path, int inode);

int addDir(unsigned char *disk, char *dirname, int inode);

int addLink(unsigned char *disk, char *lname, int file_inode, int dir_inode);

int addLinkFile(unsigned char *disk, char *lname, int link_inode, int dir_inode);

int delFile(unsigned char *disk, int to_del_inode, int parent_inode);