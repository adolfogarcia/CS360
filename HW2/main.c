#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>

typedef struct node{
          char  name[64];       // node's name string
          char  type;
   struct node *child, *sibling, *parent;
}NODE;

NODE *root = NULL, *cwd;
char line[128];                   // for gettting user input line
char command[64], pathname[64];   // for command and pathname strings
char dname[64], bname[64];        // for dirname and basename 
char *name[100];                  // token string pointers 
int  n;                           // number of token strings in pathname 
FILE *fp;

int initialize(void);
int findCmd(char *command);
int mkdir(char* pathname);
int rmdir(char* pathname);
int ls(char* pathname);
int cd(char* pathname);
int pwd(char* pathname);
int creat(char* pathname);
int rm(char* pathname);
int reload(char* pathname);
int reload(char* pathname);
int save(char* pathname);
int menu(char* pathname);
int quit(char* pathname);
char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat", "rm","reload", "save", "menu", "quit", NULL};

int main()
{
    
    int (*fptr[ ])(char *)={(int (*)())mkdir,rmdir,ls,cd,pwd,creat,rm,reload, save, menu, quit};

    initialize();
   // initialize root node of the file system tree
    while(1)
    {
        strcpy(pathname, " ");
        printf("Enter a command: \n");
        //get user input line = [command pathname];
        fgets(line, 128, stdin);  // get at most 128 chars from stdin

        line[strlen(line) -1] = 0; // kill \n at end of line
        sscanf(line, "%s %s", command, pathname);
    
        //identify the command;
        int commandIndex = findCmd(command);

        //execute the command;
    
        /// check for invalid command
        if(commandIndex < 0 || commandIndex > 10)
        {
            printf("Invalid command!\n");
        }
        else
        {
            int result = fptr[commandIndex](pathname);
        }

        /// 10 == quit so break
        if(commandIndex== 10)
        {
            break;
        }

    }

}

int initialize(void)
{
   // printf("Entering Initialize!\n");
    root = (NODE*) malloc(sizeof(NODE));
    root->child = NULL;
    root->parent = root;
    root->sibling = NULL;
    root->name[0] = '/';
    root->type = 'D';
    cwd = root;
    //printf("Exititing initialize!\n");
}

int deleteTreeHelper(NODE* ptr)
{
    if(!ptr)
    {
        return 1;
    }

    NODE* childPtr = ptr->child;
    NODE* siblingPtr = ptr->sibling;

    free(ptr);

    deleteTreeHelper(childPtr);
    deleteTreeHelper(siblingPtr);
}

int deleteTree(void)
{
    deleteTreeHelper(root);
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

NODE* makeNode(char* name, char type)
{
    NODE* newNode = (NODE*) malloc(sizeof(NODE));
    newNode->child= NULL;
    newNode->sibling = NULL;
    newNode->parent = NULL;
    strcpy(newNode->name, name);
    newNode->type = type;
}

NODE *search_child(NODE *parent, char *name)
{ 
    NODE* ptr = parent->child;
   
   while(ptr)
   {
      
        if(strcmp(ptr->name, name) == 0)
        {
            return ptr;
        }
       
        ptr = ptr->sibling;
        if(!ptr)
        {
            break;
        }
        
   }

   return 0;
}


NODE *path2node(char *pathname)
{
    NODE* start = NULL;
    int n  =0;
    
    if(pathname[0] == '.')
    {
        return cwd;
    }
    if(pathname[0] == '/')
    {
    
            start = root;
            
    }
    else
    {
            start = cwd;
   
    }

    // NOTE: pass a copy of pathname
    n = tokenize(pathname);

    NODE *p = start;

    for (int i=0; i < n; i++){
    
        
        p = search_child(p, name[i]);
        
        if(!p) 
        {
    //printf("path2Node! Node %s not found!\n", name[i]);
            return 0;
        }
    }
   
    return p;
   
}


int findCmd(char *command)
{

    int i = 0;
    while(cmd[i])
    {
      
        if (strcmp(command, cmd[i]) == 0)
        {

            return i;
        }
        // found command: return index i
        i++;
    }
    return -1;
    // not found: return -1
}

char dname[64], bname[64]; // for decomposed dir_name and base_name 

int dbname(char *pathname)
{
    char temp[128];  // dirname(), basename() destroy original pathname
    strcpy(temp, pathname);
    strcpy(dname, dirname(temp));
    strcpy(temp, pathname);
    strcpy(bname, basename(temp));
}

//char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat", "rm","reload", "save", 'menu', "quit", NULL};

int insertNode(NODE* ptr, char* pathname, char type)
{
    //printf("\nENTERING insertNode!\n");

    NODE* newNode = makeNode(bname, type); 
    newNode->parent = ptr;

    if(!ptr->child)
    {
        ptr->child = newNode;
       // newNode->parent = ptr;
    //    printf("\nEXITING insertNode!\n");
        return 1;
    }
    else{
        ptr = ptr->child;
        while(ptr->sibling != NULL)
        {
            ptr = ptr->sibling;
        }
        ptr->sibling = newNode;
        //newNode->parent = ptr;
    //    printf("\nEXITING insertNode!\n");

        return 1;
    }
    //printf("\nExiting insertNode!\n");
    return 0;
}

int removeNode(NODE* ptr)
{
    //printf("REMOVENODE! ptr name: %s\n", ptr->name);
    NODE* temp = ptr->parent->child;

    if(temp == ptr)
    {
        ptr->parent->child = ptr->sibling;
    }
    else
    {
        while(temp->sibling != ptr)
        {
            temp = temp->sibling;
        }
        temp->sibling = ptr->sibling;
    }

    free(ptr);
}

int mkdir(char* pathname)
{
    dbname(pathname);
    NODE* ptr = NULL;

    //int n = tokenize(pathname);

    //printf("mkdir! pathname: %s dname: %s  bname: %s\n", pathname, dname, bname);

    //find node to directory we wanna insert in
    ptr = path2node(dname);

    if(!ptr || ptr->type == 'F')
    {
        printf("ERROR! Could not find directory %s\n", dname);
        return 0;
    }    
    else{
        // MAKE DISTINCTION BETWEEN INSERTING RELATIVE INSERTs

        ///not found, ie insert here
        if(search_child(ptr, bname) == 0)
        {
            insertNode(ptr, bname, 'D');
        }
    }

    return 0;
}

int rmdir(char * pathname)
{
    //printf("Entering rmdir! Pathname: %s\n", pathname);

    NODE* ptr = NULL;

    // find where node in question is at
    ptr = path2node(pathname);
    //printf("ptr name: %s\n", ptr->name);


    if(!ptr || ptr->type == 'F')
    {
        printf("ERROR! Could not find directory %s\n", dname);
        return 0;
    }    
    else
    {
        if(ptr->child)
        {
            printf("Directory not empty! Cannot remove directory \n");
          //  printf("Exiting rmdir!\n");
            return 0;
        }
        else
        {
            removeNode(ptr);
            //printf("removed node! Exiting rmdir!\n");
            return 1;
        }

    }

    //printf("Exiting rmdir!\n");
    return 0;
}

int ls(char* pathname)
{
    NODE* temp = cwd->child;

    
    while(temp)
    {
        printf("[TYPE: %c   NAME: %s] \n", temp->type, temp->name);
        temp = temp->sibling;
    }
    printf("\n");
    return 0;
}

int cd(char* pathname)
{
    NODE* ptr = NULL;

    //find node to directory we wanna insert in
    ptr = path2node(pathname);

    if(strcmp(pathname, "..") == 0)
    {
        cwd  = ptr->parent;
        return 1;
    }
    else if(strcmp(pathname, " ") == 0)
    {
        cwd = root;
        return 1;
    }

    if(!ptr || ptr->type == 'F')
    {
        printf("ERROR! Could not find directory %s\n", dname);
        return 0;
    }    

    cwd = ptr;
    //printf("cwd parent = %s\n", cwd->parent->name);
    return 1;
}

int pwdHelper(NODE* ptr)
{
    if(ptr == root)
    {
        printf("/");
        return 0;
    }
    // if(ptr->parent->sibling != ptr)
    // {
    //     //printf("pwdhelper ptr->name: %s!\n", ptr->name);
    //     char * tempStr = ptr->name;
    //     //printf("recursing!\n");
    pwdHelper(ptr->parent);
    printf("%s/", ptr->name);
    // }
    // else{
    //     pwdHelper(ptr->parent);
    // }
    return 0;
}

int pwd(char* pathname)
{
    //printf("entering pwd!\n");
     NODE * temp = cwd;
    // char * tempStr = cwd->name;
    
    pwdHelper(temp);
    // if(cwd != root)
    //     printf("%s/\n", cwd->name);
    printf("\n");
    return 0;
}

int creat(char* pathname)
{
    dbname(pathname);
    NODE* ptr = NULL;

    //int n = tokenize(pathname);

    //printf("mkdir! pathname: %s dname: %s  bname: %s\n", pathname, dname, bname);

    //find node to directory we wanna insert in
    ptr = path2node(dname);

    if(!ptr || ptr->type == 'F')
    {
        printf("ERROR! Could not find directory %s\n", dname);
        return 0;
    }    
    else{
        // MAKE DISTINCTION BETWEEN INSERTING RELATIVE INSERTs

        ///not found, ie insert here
        if(search_child(ptr, bname) == 0)
        {
            insertNode(ptr, bname, 'F');
        }
    }

    return 0;
}

int rm(char* pathname)
{


   // printf("Entering rmdir! Pathname: %s\n", pathname);

    NODE* ptr = NULL;

    // find where node in question is at
    ptr = path2node(pathname);
    //printf("ptr name: %s\n", ptr->name);


    if(!ptr || ptr->type == 'D')
    {
        printf("ERROR! Could not find file %s\n", dname);
        return 0;
    }    
    else
    {
       
            removeNode(ptr);
           // printf("removed node! Exiting rmdir!\n");
            return 1;

    }

   // printf("Exiting rmdir!\n");
    
    return 0;
}

int reload(char* pathname)
{
    FILE* infile = fopen("saveFile.txt", "r");

    if(!infile)
    {
        return 0;
    }

  //  printf("lets load this sucker up!\n");
//
    char line[100];
    char type[1];
    char pname[100];
    char name[100];

    //// get root node
    fgets(line, 100, infile);
  //  printf("LINE: %s\n", line);
   // sscanf("%c %s %s", type, pname, name);

    deleteTree();
    initialize();
    strcpy(line, " ");

    while(fgets(line, 100, infile))
    {
      //  printf("LINE: %s\n", line);
        sscanf(line, "%s %s %s", type, pname, name);
      //  printf("TYPE: %s  PATHNAME: %s\n", type, pname);
        if(type[0] == 'D')
        {
            mkdir(strcat(pname, name));
        }
        else
        {
            creat(strcat(pname, name));
        }
        strcpy(line, " ");
    }

    fclose(infile);
    return 1;
}

int saveHelper(NODE* ptr, FILE* outfile, char* pathname)
{
    if(!ptr)
    {
        return 1;
    }

    fprintf(outfile, "%c %s %s\n", ptr->type, pathname, ptr->name);
    char nextPath[100];
    strcpy(nextPath, pathname);
    strcat(nextPath, ptr->name);
   // printf("nextPath: %s\n", nextPath);
    if(strcmp(ptr->name, "/") != 0)
    {
        strcat(nextPath, "/");
    }
    saveHelper(ptr->child, outfile, nextPath);
    saveHelper(ptr->sibling, outfile, pathname);

}
 
int save(char* pathname)
{
    FILE* outfile = fopen("saveFile.txt", "w");
    saveHelper(root, outfile, "");
    fclose(outfile);
    return 0;
}

int menu(char* pathname)
{
    printf("|--------- MENU ----------|\n\n");
    printf("mkdir 'pathname':  create a new directory\n\n");
    printf("rmdir 'pathname':  delete a directory\n\n");
    printf("creat 'pathname':  create a new file\n\n");
    printf("rm 'pathname':  delete a file\n\n");
    printf("ls:  list contents of current working directory\n\n");
    printf("cd 'pathname':  move into specified pathname\n\n");
    printf("pwd:  display current working directory\n\n");
    printf("save:  save directory tree as text file\n\n");
    printf("reload:  create directory tree based on text file\n\n");
    printf("menu':  show all commands and their function\n\n");
    printf("quit:  exit program\n\n");
    printf("|------------------------|\n\n");
    return 0;
}

int quit(char* pathname)
{
    printf("Quitting program!\n");
    deleteTree();
    return 0;
}

