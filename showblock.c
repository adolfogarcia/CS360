#include <stdio.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <unistd.h>
#include <string.h>

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
char* name[100];

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

    char buf[BLKSIZE];
    char dirname[EXT2_NAME_LEN]
    // int block0 = ip->i_block[0];
    // get_block(fd, block0, buf);
    char* cp;
    DIR * dp;
    int dev = fd;

    int i  = 0;
    for(i = 0; i < 12; i++)
    {
        if(ip->i_block[i] == 0)
            break;
        get_block(dev, ip->i_block[i], buf);
        dp = (DIR *) buf;
        cp = buf;

        while(cp < buf +  1024)
        {
            dirname[EXT2_NAME_LEN];
            strcpy(dirname, dp->name);
            dirname[dp->name_len] = '\0';

            if(strcmp(dirname, name) == 0)
            {
                return dp->inode;
            }

            cp += dp->rec_len;
            dp = (DIR*) cp;

        }
    }
    return 0;
}

int tokenize(char *pathname)
{
    //decompose pathanme into token strings pointed by
    //      name[0], name[1], ..., name[n-1] 
    //return n = number of token strings

    int tokenCount = 0;
    char *s;
    s = strtok(pathname, "/");  // first call to strtok()

    if(!s)
    {
        return 0;
    }

    while(s)
    {
        name[tokenCount] = s;
        tokenCount++;
        s = strtok(0, "/");  // call strtok() until it returns NULL
    }

    return tokenCount;
}

char *disk = "mydisk";
int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("Usage: showblock diskimage /a/b/c/d");
        return 0;
    } 

     fd = open(argv[1], O_RDONLY);

    tokenize(argv[2]);
}