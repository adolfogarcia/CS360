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

int fd, dev;
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
    char dirname[EXT2_NAME_LEN];
    // int block0 = ip->i_block[0];
    // get_block(fd, block0, buf);
    char* cp;
    DIR * dp;
    dev = fd;

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
            printf("Dir : %s\n ", dirname);
            printf("Name to find: %s\n", name);

            if(strcmp(dirname, name) == 0)
            {
                printf("\nFOUND!\n");
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

void printInfo(INODE* ip)
{
    printf("In printInfo!\n");
    int i = 0;
    /// DIRECT BLOCKS
    printf("********* DIRECT BLOCKS *********\n");
    for(i = 0; i < 12; i++)
    {
        printf("Block %d: %d\n", i + 1, ip->i_block[i]);
    }
    printf("********* INDIRECT BLOCKS *******\n");
    printf("Block 13: %d\n", ip->i_block[12]);
    printf("********* DOUBLE INDIRECT BLOCKS *******\n");
    printf("Block 14: %d\n", ip->i_block[13]);
}

void printDir(INODE *ip)
{
    printf("in printDir!\n");
    char buf[BLKSIZE];
    char dirname[EXT2_NAME_LEN];
    // int block0 = ip->i_block[0];
    // get_block(fd, block0, buf);
    char* cp;
    DIR * dp;
    dev = fd;

    int i  = 0;
    for(i = 0; i < 12; i++)
    {
        if(ip->i_block[i] == 0)
        {
            printf("Break out of printDir\n");
            break;
        }
    
        get_block(dev, ip->i_block[i], buf);
        dp = (DIR *) buf;
        cp = buf;

        while(cp < buf +  1024)
        {
            dirname[EXT2_NAME_LEN];
            strcpy(dirname, dp->name);
            dirname[dp->name_len] = '\0';
            printf("Dir : %s\n", dirname);
            printf(" %4d %4d %4d %s\n", dp->inode, dp->rec_len, dp->name_len, dirname);
        
            cp += dp->rec_len;
            dp = (DIR*) cp;

        }
    }
    //return 0;
}

char *disk = "mydisk";
int main(int argc, char *argv[])
{
    char buf[BLKSIZE];
    if(argc < 3)
    {
        printf("Usage: showblock diskimage /a/b/c/d\n");
        return 0;
    } 

    fd = open(argv[1], O_RDONLY);

    if(!fd)
    {
        printf("Couldn't open %s\n", argv[1]);
        return 0;
    }

    int n =tokenize(argv[2]);

    // char buf[BLKSIZE];
    // get_block(fd, 2, buf);
    // GD* gp = (GD*) buf;
    // get_block(fd, gp->bg_inode_table, buf);
    // INODE* ip = (INODE*) buf + 1;

    //Start from the root inode #2, which is stored in ip
    get_block(fd, 2, buf);
    GD* gp = (GD *)buf;
    int iblk = gp->bg_inode_table; 
    get_block(fd, iblk, buf);
    INODE * ip = (INODE *)buf + 1;    

    int ino, blk, offset, i;

    ///TODO

    char ibuf[BLKSIZE];
    printDir(ip);

    // WHat is n?
    for(i = 0; i < n; i++)
    {
        printf("loop %d\n", i);
        ino = search(ip, name[i]);
        printf("ino: %d\n", ino);
        if(ino == 0)
        {
            printf("Can't find %s\n", name[i]);
            //exit(1);
            return 0;
        }

        // Mailman's algorithm: Convert (dev, ino) to inode pointer
        blk = (ino - 1) / 8 + iblk;
        offset = (ino - 1) % 8;
        get_block(dev, blk, ibuf);
        ip = (INODE*)ibuf + offset;
    }


    printInfo(ip);
}