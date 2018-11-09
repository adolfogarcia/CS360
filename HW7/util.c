/************** util.c file ****************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

#include "type.h"
extern char *name[64]; // assume at most 64 components in pathnames


/**** globals defined in main.c file ****/

int get_block(int fd, int blk, char buf[ ])
{

  lseek(fd,(long)blk*BLKSIZE, 0);
   read(fd, buf, BLKSIZE);

}

int put_block(int fd, int blk, char buf[ ])
{
  lseek(fd, (long)blk*BLKSIZE, 0);
  write(fd, buf, BLKSIZE);
}  

int tokenize(char *pathname)
{
    char c[100], f[100];
    //printf("tokenizing!\n");
    //decompose pathanme into token strings pointed by
    //      name[0], name[1], ..., name[n-1] 
    //return n = number of token strings

    int tokenCount = 0;
    char *s;

   
        s = strtok(pathname, "/");  // first call to strtok()
        //printf("token: %s\n", s);
        if(!s)
        {
            return 0;
        }

        while(s)
        {
            name[tokenCount] = s;
            tokenCount++;
            s = strtok(0, "/");  // call strtok() until it returns NULL
           // printf("token: %s\n", s);
            // fgets(f, 128, stdin);
            // memset(f, 0, 100);
        }

    //}

    return tokenCount;
}

MINODE *iget(int dev, int ino)
{
    //printf("iget()\n");
    //printf("iget(%d %d)\n ", dev, ino);
    MINODE * mip;
    int i;
  // return minode pointer to loaded INODE

  /*(1). Search minode[ ] for an existing entry (refCount > 0) with 
       the needed (dev, ino):
       if found: inc its refCount by 1;
                 return pointer to this minode;*/
    for ( i=0; i < NMINODE; i++){
        // if(minode[i].refCount > 0)
        // {
            if(minode[i].dev == dev && minode[i].ino == ino)
            {
                minode[i].refCount++;
               // printf("found INODE=[%d %d] at minode[%d], return adr\n", dev, ino, i);
              //  printf("refCount: %d\n", minode[i].refCount);
                return &minode[i];
            }
        //      }
    }

  /*(2). // needed entry not in memory:
       find a FREE minode (refCount = 0); Let mip-> to this minode;
       set its refCount = 1;
       set its dev, ino*/
    for ( i=0; i < NMINODE; i++){
        if(minode[i].refCount == 0)
        {
            minode[i].refCount = 1;
            mip = &minode[i];
           // printf("MIP refcount: %d\n", mip->refCount);
            mip->dev = dev;
            mip->ino = ino;
            break;
        }
    }

    //*(3). load INODE of (dev, ino) into mip->INODE:
        /// inode start is root inode #2 im pretty sure
    char buf[BLKSIZE];

    get_block(dev, 2, buf);
    GD* gp = (GD *)buf;
    int inode_start = gp->bg_inode_table; 

    get_block(dev, ino, buf);
    INODE * ip = (INODE *)buf + 1;    

    int  blk, offset;
    // get INODE of ino a char buf[BLKSIZE]    
    blk    = (ino-1) / 8 + inode_start;
    offset = (ino-1) % 8;

    //printf("iget: ino=%d blk=%d offset=%d\n", ino, blk, offset);

    get_block(dev, blk, buf);
    ip = (INODE *)buf + offset;
    mip->INODE = *ip;  // copy INODE to mp->INODE

    //printf("load INODE=[%d %d] into minode[%d]\n", dev, ino, i);
    //printf("minode[i] refCount: %d\n", mip->refCount);
    return mip;
}

int iput(MINODE *mip) // dispose a used minode by mip
{
  //  printf("iput()\n");
    char buf[BLKSIZE];
    get_block(dev, 2, buf);
    GD* gp = (GD *)buf;
    int inode_start = gp->bg_inode_table; 

    mip->refCount--;

    if (mip->refCount > 0) return;
    if (!mip->dirty)       return;

    // Write YOUR CODE to write mip->INODE back to disk
    int ino = getino( ".");
    int pos = (ino - 1) / 8 + inode_start;
    int offset = (ino - 1) / 8;
    get_block(dev, pos, buf);
    INODE * ip = (INODE*) buf + offset;
    *ip = mip->INODE;
    put_block(dev, pos, buf);

    
} 

int search(MINODE *ip, char *name)
{
    //printf("search() looking for %s\n", name);
    char buf[BLKSIZE];
    char dirname[EXT2_NAME_LEN];
    // int block0 = ip->i_block[0];
    // get_block(fd, block0, buf);
    char* cp;
    DIR * dp;
  //  dev = fd;

    int i  = 0;
    for(i = 0; i < 12; i++)
    {
        if(ip->INODE.i_block[i] == 0)
        {
            printf("No more blocks! %s not found!\n", name);
            break;
        }

        get_block(dev, ip->INODE.i_block[i], buf);
        dp = (DIR *) buf;
        cp = buf;

        while(cp < buf +  1024)
        {
            dirname[EXT2_NAME_LEN];
            strcpy(dirname, dp->name);
            dirname[dp->name_len] = '\0';
         //   printf("Dir : %s\n ", dirname);
            // printf("Name to find: %s\n", name);

            if(strcmp(dirname, name) == 0)
            {
            //    printf("\nFOUND! i value: %d\n", i);
                return dp->inode;
            }

            cp += dp->rec_len;
            dp = (DIR*) cp;

        }
    }
    return 0;
}


// retrun inode number of pathname
int getino(char *pathname)
{ 
    //printf("getino()\n");
   // SAME as LAB6 program: just return the pathname's ino;
    char buf[BLKSIZE], temp[BLKSIZE];
    char* cp;
    DIR * dp;

    int n = tokenize(pathname);
    
    get_block(dev, 2, buf);
    GD* gp = (GD *)buf;
    int inode_start = gp->bg_inode_table; 
    get_block(dev, inode_start, buf);
    INODE* ip = (INODE*) buf + 1;

    int ino = 0, blk, offset, i;
    for(i = 0; i < n; i++)
    {
      //  printf("looking for %s!\n", name[i]);
        ino = search(ip, name[i]);
        if (ino==0){
                printf("can't find %s\n", name[i]);
                break;
                // exit(1);
        }
        // get_block(dev, mip->INODE.i_block[0], buf);
        // dp = (DIR *)buf; //set the dir
        // cp = buf; //set the cp
        // while (cp < buf + BLKSIZE)
        // {
        //     strncpy(temp, dp->name, dp->name_len);
        //     temp[dp->name_len] = 0;

        //     printf("Dir : %s\n ", temp);
        //     printf("Name to find: %s\n", name[n-1]);

        //     if(strcmp(temp, name[n-1]) == 0)
        //     {
        //         // if (i == n - 1)
        //         // {
        //             printf("\nFOUND!\n");
        //             return dp->inode;
        //         //}
        //         // mp = iget(dev, dp->inode);
        //         // i++;
        //         // break;
        //     }
        //     memset(temp, 0, 100);
        //     cp += dp->rec_len;
        //     dp = (DIR *) cp;
        ip = iget(dev, ino);
    }
    // if(ino != 0)
    //     puts("found ino!");
    return  ino;
}

void printDir(INODE ip)
{
   // printf("PrintDir!\n");
    char buf[BLKSIZE];
    char buf2[BLKSIZE];
    char dirname[EXT2_NAME_LEN];
    // int block0 = ip->i_block[0];
    // get_block(fd, block0, buf);
    char* cp;
    DIR * dp;
    dev = fd;

    int i  = 0;
    // for(i = 0; i < 12; i++)
    // {
    //     if(ip->i_block[i] == 0)
    //     {
    //         printf("Break out of printDir\n");
    //         break;
    //     }
    
        get_block(dev, ip.i_block[i], buf);
        dp = (DIR *) buf;
        cp = buf;

        struct stat fileStat;
        char cwdName[256];
        while(cp < buf +  1024)
        {
            dirname[EXT2_NAME_LEN];
            strcpy(dirname, dp->name);
            dirname[dp->name_len] = '\0';
            getcwd(cwdName, 256);
            // strcat(cwdName, "/");
            // strcat(cwdName, "mydisk");
            strcat(cwdName, "/");
            strcat(cwdName, dirname);
           // printf("cwd: %s  dirname: %s\n", cwdName, dirname);
            if(!stat(cwdName, &fileStat))
            {
                //printf("stat not found!\n");
            }

           // printf("%s\n", dirname);
            printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
            printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
            printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
            printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
            printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
            printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
            printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
            printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
            printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
            printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
            printf(" %d %d %s\n", fileStat.st_size, fileStat.st_ino, dirname);
            //printf(" %4d %4d %4d %s\n", dp->inode, dp->rec_len, dp->name_len, dirname);
        
            cp += dp->rec_len;
            dp = (DIR*) cp;
             //memset(cwdName, 0, 256);
             strcpy(cwdName, "");
              memset(dirname, 0, 255);
        }
    //}
    //return 0;
}


int ls_dir(char* dirname)
{
    char* cp;
  

     char buf[BLKSIZE];
    
    if(!dirname)
    {
        MINODE *  mip = running->cwd;
        printDir(mip->INODE);
    }
  
    else
    {
        //printf("GONNA LOOK for %s\n", name[1]);
     
        int ino = getino(dirname);
        //printf("INO: %d\n", ino);
        if(ino <= 0)
        {
            printf("%s Not found!!!!\n", name[1]);
            return;
        }

        MINODE* mip = iget(dev, ino);
        printDir(mip->INODE);
      
    }

     return;
}

int cd(char* dirname)
{
    MINODE* mip = running->cwd;
    int ino = 0;

    if(dirname)
    {
        ino = search(mip,dirname);
        //printf("INO: %d\n", ino);
        if(ino > 0)
        {
            mip = iget(dev, ino);

            if(S_ISDIR(mip->INODE.i_mode))
            {
                iput(running->cwd);
                running->cwd = mip;
            }

        }
        
    }
    else
    {
        iput(running->cwd);
        running->cwd = root;
    }
}

int rpwd(MINODE * wd)
{
    if(wd == root)
    {
        return 0;
    }

    char buf[BLKSIZE], dirname[BLKSIZE];
    //char name[BLKSIZE];
    int my_ino = 0, parent_ino = 0;
    get_block(dev, wd->INODE.i_block[0], buf);
    DIR* dp = (DIR *) buf;
    char* cp = buf;

    while(cp < buf +  1024)
    {
        dirname[EXT2_NAME_LEN];
        strcpy(dirname, dp->name);
        dirname[dp->name_len] = '\0';
//printf("Dir : %s\n ", dirname);
        // printf("Name to find: %s\n", name);

        if(strcmp(dirname, ".") == 0)
        {
          
            my_ino = dp->inode;
            //  printf("\my_ino found! %d\n", my_ino);
        }
        if(strcmp(dirname, "..") == 0)
        {
            
            parent_ino = dp->inode;
            // printf("\n parent_ino found! %d\n", parent_ino);
        }

        cp += dp->rec_len;
        dp = (DIR*) cp;
    }
    MINODE* pip =iget(dev, parent_ino);
    get_block(dev, pip->INODE.i_block[0], buf);
    dp = (DIR *) buf;
    cp = buf;

    ///loop searching for my_ino == dp->inode
    while(cp < buf +  1024)
    {
       // printf("Looping!");
         dirname[EXT2_NAME_LEN];
        strcpy(dirname, dp->name);
        dirname[dp->name_len] = '\0';
       //  printf("Dir : %s\n ", dirname);
        // printf("Name to find: %s\n", name);

        if(dp->inode == my_ino)
        {
            //printf("FOUND MY_INO\n");
            break;
        }
        cp += dp->rec_len;
        dp = (DIR*) cp;
    }

    // strcpy(dirname, dp->name);
    // dirname[dp->name_len] = '\0';
    // printf("DIRNAME: %s\n", dirname);

    pip =iget(dev, parent_ino);
    rpwd(pip);

    printf("/%s", dirname);
}

int pwd(MINODE *wd)
{
    if(wd == root)
    {
        printf("/");
    }
    else
        rpwd(wd);
    printf("\n");
}

int quit()
{
    int i =0;
    for ( i=0; i < NMINODE; i++){
        // if(minode[i].refCount > 0)
        // {
            if(minode[i].refCount > 0 && minode[i].dirty)
            {
                iput(&minode[i]);
            }
        //      }
    }
    exit(0);
}