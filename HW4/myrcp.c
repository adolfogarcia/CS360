#include <stdio.h>       // for printf()
#include <stdlib.h>      // for exit()
#include <string.h>      // for strcpy(), strcmp(), etc.
#include <libgen.h>      // for basename(), dirname()
#include <fcntl.h>       // for open(), close(), read(), write()

// for stat syscalls
#include <sys/stat.h>
#include <unistd.h>

// for opendir, readdir syscalls
#include <sys/types.h>
#include <dirent.h>


int main(int argc, char *argv[])
{
    char f1[256];
    char f2[256];
  if (argc < 3)
  {
      //print usage and exit;
      printf("Not enough arguments! myrcp f1 f2 \n");
  }


  /// check argv[1] and argv[2] to see if they are relative or absolute path
  /// PSEUDOCODE:
  /// if(argv[1][0] != '/')
  /// { it is relative}



    if(argv[1][0] != '/')
    {
        getcwd(f1, 256);
        strcat(f1, "/");
        strcat(f1, argv[1]);
    }
    else{
        /// is absolute
        strcat(f1, argv[1]);
    }

    if(argv[2][0] != '/')
    {
        getcwd(f2, 256);
        strcat(f2, "/");
        strcat(f2, argv[2]);
    }
    else
    {
        strcat(f2, argv[2]);
    }

    return myrcp(f1,f2);
}

int myrcp(char *f1, char *f2)
{
//    1. stat f1;   if f1 does not exist ==> exit.
//                  f1 exists: reject if f1 is not REG or LNK file
    struct stat file1;
    int status1 = stat(f1, &file1);
    if( status1 == -1)
    {
        //// f1 does not exist
        //printf("%s does not exist!\n", f1);
        return 0;
    }
    else if(status1 == 0)
    {
        if((S_ISREG(file1.st_mode) == 0) )
        {
           // printf("file1 is a reg file1\n");
        }

        // f1 exists: reject if f1 is not REG or LNK file
        if((S_ISREG(file1.st_mode) != 0) && (S_ISLNK(file1.st_mode) != 0))
        {
            //printf("Line 75 - File %s is not REG or LNK file!\n", f1);
            return 0;
        }
    }
//    2. stat f2;   reject if f2 exists and is not REG or LNK file
    struct stat file2;
    if(stat(f2, &file2) < 0)
    {
        ///f2 does not exist
       // printf("%s does not exist!\n", f2);
        //return 0;
    }
    else
    {
        //  reject if f2 exists and is not REG or LNK file
        if((S_ISREG(file2.st_mode) != 0) && (S_ISLNK(file2.st_mode) != 0))
        {
            printf("Line 92 - File %s is not REG or LNK file!\n", f2);
            return 0;
        }
    }

//  3. if (f1 is REG){
//          if (f2 non-exist OR exists and is REG)
//             return cpf2f(f1, f2);
//  else // f2 exist and is DIR
//             return cpf2d(f1,f2);
//       }
    if(S_ISREG(file1.st_mode))
    {
        if(access(f2, F_OK) == -1 || S_ISREG(file2.st_mode))
        {
            return cpf2f(f1, f2);
        }
        else
        {
            //printf("f2 is a dir\n");
            // f2 exist and is DIR
            return cpf2d(f1,f2);
        }
    }

//    4. if (f1 is DIR){
// 	if (f2 exists but not DIR) ==> reject;
//         if (f2 does not exist)     ==> mkdir f2
// 	return cpd2d(f1, f2);
//       }
    if(S_ISDIR(file1.st_mode))
    {
        if( access(f2, F_OK) != -1 && !S_ISDIR(file2.st_mode))
        {
            printf("REJECT!\n");
            return 0;
        }
        if(access(f2, F_OK) == -1)
        {
            //mkdir f2
            mkdir(f2, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
        return cpd2d(f1, f2);
    }


}

// cp file to file
int cpf2f(char *f1, char *f2)
{
//   1. reject if f1 and f2 are the SAME file
    struct stat file1;
    if( stat(f1, &file1) < 0)
    {
        //// f1 does not exist
        printf("%s does not exist!\n", f1);
        return 0;
    }
    else
    {
        // f1 exists: reject if f1 is not REG or LNK file
        if((S_ISREG(file1.st_mode) != 0) && (S_ISLNK(file1.st_mode) != 0))
        {
           // printf("Line 160 - File %s is not REG or LNK file!\n", f1);
            return 0;
        }
    }

    struct stat file2;
    if(lstat(f2, &file2) < 0)
    {
        ///f2 does not exist
        //printf("line 169 - f2 %s does not exist!\n", f2);
        //printf("%d\n", access(f2, F_OK));
    }
    else
    {
        //  reject if f2 exists and is not REG or LNK file
        if((S_ISREG(file2.st_mode) != 0) && (S_ISLNK(file2.st_mode) != 0))
        {
           // printf("Line 177 - File %s is not REG or LNK file!\n", f2);
            return 0;
        }
    }

    if(file1.st_ino == file2.st_ino)
    {
        /// same file 
        printf("These are the same files!\n");
        return 0;
    }
//   2. if f1 is LNK and f2 exists: reject
    if(S_ISLNK(file1.st_mode) && access(f2, F_OK) != -1)
    {
        printf("f1 is lnk and f2 exists\n");
        return 0;
    }
//   3. if f1 is LNK and f2 does not exist: create LNK file f2 SAME as f1
    if(S_ISLNK(file1.st_mode) == 0 && access(f2, F_OK) == -1)
    {
        //printf("create f2!\n");
        symlink(f1, f2);
        //create LNK file f2 SAME as f1
    }
//   4:
//      open f1 for READ;
    int fd1 = open(f1, O_RDONLY);
//      open f2 for O_WRONLY|O_CREAT|O_TRUNC, mode=mode_of_f1;
    int fd2 = open(f2, O_WRONLY|O_CREAT|O_TRUNC, 0644);
//      copy f1 to f2
    char temp[100];
    int numberOfBytes = 0;
    numberOfBytes = read(fd1, temp, 100);

    while(numberOfBytes != 0)
    {
        //printf("reading to %s!\n", f2);
        write(fd2, temp, numberOfBytes);
        numberOfBytes = read(fd1, temp, 100);
    }   

    close(fd1);
    close(fd2);
}

int cpf2d(char *f1, char *f2)
{
    int found  = 0;
//   1. search DIR f2 for basename(f1) 
//      (use opendir(), readdir())
     // x=basename(f1); 
     char f1temp[100];
     strcpy(f1temp, f1);
    char* bname = basename(f1temp);
   // printf("bname: %s\n", bname);

    char temp[100];
    strcpy(temp, f2);
   // printf("Opening %s!\n", dirname(temp));
    DIR* dir = opendir((temp));
    if(dir)
    {
        printf("Directory %s could  be opened!\n", temp);
    }

    struct dirent *dp;
    dp = readdir(dir);
    while(dp)
    {
        //printf("dp->name : %s\n", dp->d_name);
        // if x already in f2/: 
        //      if f2/x is a file ==> cpf2f(f1, f2/x)
        //      if f2/x is a DIR  ==> cpf2d(f1, f2/x)
        if(strcmp(dp->d_name, bname) == 0)
        {
            printf("filename %s already exists in directory!\n", bname);
            found = 1;
            struct stat file1;
            if( stat(f1, &file1) < 0)
            {
                //// f1 does not exist
               // printf("line 246 - %s does not exist!\n", f1);
                return 0;
            }
            
            struct stat testFile;
            strcat(f2, "/");
            strcat(f2, bname);
            stat(f2, &testFile);

            if(S_ISDIR(testFile.st_mode))
            {
               // printf("line 256\n");
                cpf2d(f1,f2);
            }
            else// if (S_ISLNK(testFile.st_mode) || S_ISREG(testFile.st_mode))
            {
              //  printf("bname is a file! f1 %s f2 %s\n", f1, f2);
                cpf2f(f1, f2);
            }  
        }
        dp = readdir(dir);
    }

    // if x not in f2/ ==> cpf2f(f1, f2/x)
   // printf("line 269\n");
    if(found == 0)
    {
        /// this isn't working, f2 isn't in the directory, it's f1??
        //printf("f1 not found in f2\n");
        strcat(f2, "/");
        strcat(f2, bname);
       /// printf("About to call cpf2f in cpf2d! f1: %s   f2: %s\n",f1,f2);
        cpf2f(f1, f2);
    }
}

int cpd2d(char *f1, char *f2)
{
    char path[100];
    strcpy(path, f1);
    cpd2dHelper(path, f1, f2);
}

int cpd2dHelper(char* path, char *f1, char *f2)
{
   int x = determineValidPaths(f1, f2) ;

    if(x == 0)
    {
        /// invalid paths
        printf("INVALID PATHS!!!!!!!  can't copy over f1 to f2\n");
        return 0;
    }
   // printf("Path: %s\n", path);
     // recursively cp dir into dir

    /// PSEUDOCODE:
    /// while(!(dp = readdir(dir)))
    ///     if(!ISDIR(dp))
    ///         cpf2d(dp)
    ///     else 
    ///         cpd2d(dp->name, f2)
    // char pathClone[100];
    // strcpy(pathClone, path);

    DIR* dir1 = opendir((f1));
    
    if(dir1)
    {
       // printf("%s could  be opened!\n", f1);
    }

    struct dirent *dp;
    dp = readdir(dir1);

    char temp[100];
    char temp2[100];
    while(dp != NULL)
    {
        //printf("\nLooping! f1: %s  f2: %s\n",f1, f2);
        //printf("dp->name: %s\n", dp->d_name);
        
        struct stat file1;

        strcpy(temp, f1);
        strcat(temp, "/");
        strcat(temp, dp->d_name);

       // printf("Temp %s\n", temp);

        if( stat(temp, &file1) < 0)
        {
            //// f1 does not exist
            //printf("line 312 - %s does not exist!\n", temp);
            return 0;
        }
        if(dp->d_name[0] != '.')
        {
            ///is a file
            if(!S_ISDIR(file1.st_mode))
            {
                //printf("dp %s is a file!\n", dp->d_name);
                //printf("TEMP2: %s\n", temp2);
                strcat(temp2, f2);
                strcat(temp2, "/");
                strcat(temp2, dp->d_name);
              //  printf("About to call f2f!temp: %s temp2: %s\n", temp, temp2);
                cpf2f(temp, temp2);
            }
            else if(S_ISDIR(file1.st_mode))
            {
               // printf("dp is a dir!\n");
                strcat(temp2, f2);
                strcat(temp2, "/");
                strcat(temp2, dp->d_name);
               // printf("TEMP: %s TEMP2: %s\n", temp, temp2);
                mkdir(temp2, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                cpd2dHelper(path, temp, temp2);
            }
        }

        strcpy(temp, "");
        strcpy(temp2, "");
        dp = readdir(dir1);
    }

}

int determineValidPaths(char* path1, char* path2)
{
    char temp[100];
    strcpy(temp, path1);
    strcat(temp, "/");
    char temp2[100];
    strcpy(temp2, path2);
    strcat(temp2, "/");
    int length = strlen(temp);
    // if(!path1)
    //     printf("error!!!!!");
  //  int i;
    // printf("%s", path1);
    // int x = sizeof(path1);
    // for( i = 0; i < length; i++)
    // {
    //     printf("%c", path1[i]);
    // }
    // printf("\n");
    // // x =sizeof(path2);
    //  for( i = 0; i < length; i++)
    // {
    //     printf("%c", path2[i]);
    // }
    if(strncmp(temp, temp2,length) == 0)
    {
        if(temp[length] == temp2[length])
            return 0;
        /// invalid path
        
    }
    else{
        return 1;
    }
}