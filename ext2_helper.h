#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "ext2.h"

int isValidPath(unsigned char *disk, char *path);

int isValidDirectory(unsigned char *disk, char *path);

int isValidFile(unsigned char *disk, char *path);

int isValidNativeFile(unsigned char *disk, char *path);

int isValidLink(unsigned char *disk, char *path);

unsigned char *readDisk(char *path);

struct ext2_group_desc *get_gd(unsigned char *disk);