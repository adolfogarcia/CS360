#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>

#define BLKSIZE 1024

// define shorter TYPES, save typing efforts
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;    // need this for new version of e2fs

GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp; 

int fd;
int iblock;

int get_block(int fd, int blk, char buf[ ])
{
  lseek(fd,(long)blk*BLKSIZE, 0);
   read(fd, buf, BLKSIZE);
}

/// whhy do we even need this
int search(INODE *ip, char *name)
{
    int fd;
    char buf[1024];
    int block0 = ip->i_block[0];
    get_block(fd, block0, buf);
    char* cp = buf;
    DIR * dp = (DIR *) buf;

    while(cp < buf + 1024)
    {
        char dirname[EXT2_NAME_LEN];
        strcpy(dirname, dp->name);
        dirname[dp->name_len] = '\0';

        ///  return if file found
        cp += dp->rec_len;
        dp = (DIR*) cp;

    }
}

char *disk = "mydisk";
int main(int argc, char *argv[])
{ 
    char buf[BLKSIZE];
    int fd = open(disk, O_RDONLY);
    get_block(fd, 2, buf);
    GD* gp = (GD *)buf;
    get_block(fd, gp->bg_inode_table, buf);
    INODE* ip = (INODE *) buf + 1;

    int block0 = ip->i_block[0];
    get_block(fd, block0, buf);

    char* cp = buf;
    DIR * dp = (DIR *) buf;

    while(cp < buf + 1024)
    {
        char dirname[EXT2_NAME_LEN];
        strcpy(dirname, dp->name);
        dirname[dp->name_len] = '\0';

        /// print contents
        printf("Name: %s\n", dirname);
        printf("inode: %d\n", dp->inode);
        printf("rec_len: %d\n", dp->rec_len);
        printf("name_len: %d\n", dp->name_len);
        printf("file_type: %d\n\n", dp->file_type);

        cp += dp->rec_len;
        dp = (DIR*) cp;

    }
}