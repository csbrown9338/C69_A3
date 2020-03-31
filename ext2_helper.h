#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "ext2.h"

char *truncatePath(char *path);

unsigned char *readDisk(char *path);

struct ext2_group_desc *get_gd(unsigned char *disk);

int get_bg(unsigned char *disk, int inode);

struct ext2_dir_entry_2 *get_dir_entry(int inode);

int isValidPath(unsigned char *disk, char *path);

int isValidDirectory(unsigned char *disk, char *path);

int isValidFile(unsigned char *disk, char *path);

int isValidNativeFile(unsigned char *disk, char *path);

int isValidLink(unsigned char *disk, char *path);