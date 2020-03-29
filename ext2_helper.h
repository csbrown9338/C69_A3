#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "ext2.h"

int isValidPath(char *path);

int isValidDirectory(char *path);

int isValidFile(char *path);

int isValidNativeFile(char *path);

int isValidLink(char *path);

unsigned char *readDisk(char *path);