#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

// define shorter TYPES, save typing efforts
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;    // need this for new version of e2fs

GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp; 

#define BLKSIZE 1024

char buf[BLKSIZE];
int fd;

int get_block(int fd, int blk, char buf[ ])
{
  lseek(fd, (long)blk*BLKSIZE, 0);
  read(fd, buf, BLKSIZE);
}

//2. WRITE YOUR OWN gd.c to print information of Group Descriptor 0
/// NOTE TO SELF: fd and buf are global
int gd()
{
    get_block(fd, 2, buf);
    gp = (GD*) buf;

    // just print all the shit I guess??

    //TODO: Check for some number like the EXT2 Magic Number, but for group descriptor

    printf("\n-------- GROUP DESCRIPTOR BLOCK -----------\n");
    printf("bg_block_bitmap = %d\n", gp->bg_block_bitmap);
    printf("bg_inode_bitmap = %d\n", gp->bg_inode_bitmap);
    printf("bg_inode_table = %d\n", gp->bg_inode_table);
    printf("bg_free_blocks_count = %d\n", gp->bg_free_blocks_count);
    printf("bg_free_inodes_count = %d\n", gp->bg_free_inodes_count);
    printf("bg_used_dirs_count = %d\n", gp->bg_used_dirs_count);

}

char *disk = "mydisk";

int main(int argc, char *argv[ ])
{ 
  if (argc > 1)
     disk = argv[1];
  fd = open(disk, O_RDONLY);
  if (fd < 0){
    printf("open failed\n");
    exit(1);
  }
  gd();
}

