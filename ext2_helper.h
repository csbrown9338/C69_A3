#include <string.h>
#include "ext2.h"

char *truncatePath(char *path);

char *extractFileName(char *path);

unsigned char *readDisk(char *path);

int isValidPath(unsigned char *disk, char *path);

int isValidDirectory(unsigned char *disk, char *path);

int isValidFile(unsigned char *disk, char *path);

int isValidLink(unsigned char *disk, char *path);

int addNativeFile(unsigned char *disk, char *path, int inode);

int addDir(unsigned char *disk, char *dirname, int inode);

int addSymLink(unsigned char *disk, char *lname, char *source_name, int file_inode, int dir_inode);

int addLinkFile(unsigned char *disk, char *lname, int link_inode, int dir_inode);

int delFile(unsigned char *disk, int to_del_inode, int parent_inode);