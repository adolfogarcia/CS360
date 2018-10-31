#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <fcntl.h>
#include <time.h>

#define INFILE 1
#define OUTFILE 2
#define APPEND 3

    int pathLength = 0;

int main(int argc, char** argv, char** envp)
{
    char *cmds[100];
    char command[100];
    int argumentCount = 0;
    char *commandA;
    char *commandB;
    int pipe = 0;
   
    char * path[100];
    getPath(path, &pathLength);

    while(1)
    {
        puts("\n\nEnter a command: ");
        fgets(command, 128, stdin);
        command[strlen(command) -1] = 0; // kill \n at end of line

        //check if command is piped command
        // PSEUDOCODE:
        // STRTOK looking for pipe; if we string tok more than than once,
        // it's a piped command and we store the two separate commands in two different strings
        /// else continue the folowing code

       // printf("About to execute: %s \n", command);
        commandA = strtok(command, "|");
        commandB = strtok(NULL, "|");

        if(commandB)
        {
            pipe = 1;
        }

        if(!pipe)
        {
           
            argumentCount = tokenizeCommands(cmds, command);

            //printf("cmds[0]: %s\n", cmds[0]);
            //perform cd
            if(strcmp("cd", cmds[0]) == 0)
            {
                //printf("cmd[1] = %s\n", cmds[1]);/// no additional arguments, GO back to home directory
                if(!cmds[1] )
                {
                  //  printf("going to HOME directory!\n");
                    char * homePath = getenv("HOME");
                    chdir(homePath);
                }
                else /// cd to directory
                {
                    chdir(cmds[1]);
                }
            }
            ///// perform exit()
            else if(strcmp("exit", cmds[0]) == 0)
            {
                _exit(1);
                printf("Exiting program!\n");
                break;
            }
            ///// some other unix command
            else
            {

                //printf("single argument!\n");
                int  x =0;
                int execute = 0;
                 char temp[100];
                ///check if comand is runnable 
                for( x = 0; x < pathLength; x++)
                {
                   strcpy(temp, "");
                    strcpy(temp, path[x]);
                    strcat(temp, "/");
                    strcat(temp, cmds[0]);
                    printf("Searching in: %s\n", temp);
                    
                    
                    if(access(temp, F_OK) == 0)
                    {
                        printf("Command found!");
                        execute = 1;
                        break;  
                    }
                }
            
                if(execute)
                {
                    executeCommand(cmds, argumentCount, temp);
                    execute= 0;
                }
            }
        }
        else
        {
            printf(" commands: %s & %s\n", commandA, commandB);
            //// do pipe stuff here
             //printf("About to execute: %s and %s\n", commandA, commandB);
             ///check if runnael
             char * cmdsA[100];
             char*  cmdsB[100];
             int executeA = 0;
             int executeB = 0;

             int argcA = tokenizeCommands(cmdsA, commandA);
             int argcB = tokenizeCommands(cmdsB, commandB);

            char tempA[100];
            char tempB[100];
            //check if comand is runnable 
            for(int x = 0; x < pathLength; x++)
            {
                strcpy(tempA, "");
                strcpy(tempA, path[x]);
                strcat(tempA, "/");
                strcat(tempA, cmdsA[0]);
                printf("Searching in: %s\n", tempA);
                
                
                if(access(tempA, X_OK) == 0)
                {
                    printf("command found!\n");
                    executeA = 1;
                    break;  
                }
            }

            for(int x = 0; x < pathLength; x++)
            {
                strcpy(tempB, "");
                strcpy(tempB, path[x]);
                strcat(tempB, "/");
                strcat(tempB, cmdsB[0]);
                printf("Searching in: %s\n", tempB);
                
                
                if(access(tempB, X_OK) == 0)
                {
                    printf("command found!\n");
                    executeB = 1;
                    break;  
                }
            }
            
            if(executeA == 1 && executeB == 1)
            {
                printf("Paths: %s and %s\n", tempA, tempB);
                executePipe(cmdsA, cmdsB, argcA, argcB, tempA, tempB);
                pipe = 0;
                
            }
            pipe = 0;
        }

        strcpy(command, "");
    }

}

void getPath( char** pathArr, int *pathLength)
{
    char* path = getenv("PATH");
    int i  = 0;
    char *token = strtok(path, ":");
    //printf("%s\n", token);

    while(token)
    {
        pathArr[i] = token;
        
        token = strtok(NULL, ":");
        i++;
    }
   
   *pathLength = i;
}

int tokenizeCommands(char ** cmds, char* s)
{
    char* token;
    //printf("tokenizing!s: %s\n", s);
    token = strtok(s, " ");

    int i = 0;

    if(!token)
    {
        return 0;
    }
    while(token)
    {
        cmds[i] = token;
        token = strtok(NULL, " ");
      // printf("cmds[%d]: %s\n", i, cmds[i]);
        i++;
    }
    cmds[i] = NULL;
    
    return (i);
    //printf("exiting tokenize!\n");
}

void executeCommand(char ** cmds, int argc, char* path)
{
    //// fork child process
    int pid = fork();
    int status = 0;

    if(pid < 0)
    {
        printf("Fork failed!\n");
    }
    else if(pid > 0)
    {
        // return to parent/caller
        printf("Child forked! PID = %d\n", pid);
        pid = wait(&status);
        printf("Child process has ended! Exit Status: %d \n", status);
    }
    else
    {
        ///pid == 0
        //printf("New child!\n");
        /// pid == 0, so we are in the new child process
        /// do stuff here
       // printf("calling %s!\n", cmds[0]);
       

        ///check I/O Indirection 
        ///PSEUDOCODE: loop thru cmds array, looking for '>', '<', '>>'
        /// when found, mark the file from which we are reading, writing, etc
        int indirection = 0;
        int targetFile = 0;

        determineIndirection(cmds, argc, &indirection, &targetFile);

        // for(int i = 0; i < argc;i++)
        // {
        //     printf("cmds[%d] = %s\n", i, cmds[i]);
        // }

        int error = 0;
        switch(indirection)
        {
            case 0:
                break;
            case INFILE:
               // printf("Opening for input! targetFile=%s\n", cmds[targetFile]);
                close(0);
                error = open(cmds[targetFile], O_RDONLY);
                if(error < 0)
                    printf("Something went wrong in I/O indirection!\n");
                //dup2(error, 0);
                break;
            case OUTFILE:
               // printf("Opening for output! targetFile=%s\n", cmds[targetFile]);
                close(1);
                 error = open(cmds[targetFile], O_WRONLY|O_CREAT, 0644);
                if(error < 1)
                    printf("Something went wrong in I/O indirection!\n");
                //dup2(error, 0);
                break;
            case APPEND:
              //  printf("Opening for append! targetFile=%s\n", cmds[targetFile]);
                close(1);
                error = open(cmds[targetFile], O_WRONLY|O_APPEND|O_CREAT, 0644);
                if(error < 0)
                    printf("Something went wrong in I/O indirection!\n");
                //dup2(error, 1);
                break;
            default: 
                break;
        }


        //int x = execvp(cmds[0], cmds);
        int x = execve(path, cmds, __environ);
        //close(error);
        if(x == -1)
            printf("ERROR!\n");
    }


}

void determineIndirection(char** cmds, int argc, int *indirection, int *targetFile)
{
    for(int i = 0; i < argc; i++)
    {
        if(strcmp(cmds[i],"<") == 0)
        {
           // printf("INPUT!\n");
            *indirection = INFILE;   
            /// subjectFile (file from which we are READING) is i + 1
            cmds[i] = NULL;
            *targetFile = i + 1;
        }
        else if(strcmp(cmds[i],">") == 0)
        {
          // printf("OUTFILE!\n");
            *indirection = OUTFILE;
            cmds[i] = NULL;
            *targetFile = i + 1;
        }
        else if(strcmp(cmds[i], ">>") == 0)
        {
          //  printf("APPEND!\n");
            *indirection = APPEND;
            cmds[i] = NULL;
            *targetFile = i + 1;
        }
    }
}

void executePipe(char ** cmdsA,char ** cmdsB, int argcA, int argcB, char pathA, char pathB)
{
    int indirectionA = 0, indirectionB = 0;
    int targetFileA = 0, targetFileB = 0;
    int pd[2];
    int error = 0;
    int statusA, statusB;


    determineIndirection(cmdsA, argcA, &indirectionA, &targetFileA);
    determineIndirection(cmdsB, argcB, &indirectionB, &targetFileB);

    pipe(pd);

    int pid = fork();

    if(pid)
    {
         close(pd[0]);
         close(1);
        dup(pd[1]);

        close(pd[1]);

        // switch(indirectionA)
        // {
        //     case 0:
        //         break;
        //     case INFILE:
        //       //  printf("Opening for input! targetFile=%s\n", cmdsA[targetFileA]);
        //         close(0);
        //         error = open(cmdsA[targetFileA], O_RDONLY);
        //         if(error < 0)
        //             printf("Something went wrong in I/O indirection!\n");
        //         //dup2(error, 0);
        //         break;
        //     case OUTFILE:
        //       //  printf("Opening for output! targetFile=%s\n", cmdsA[targetFileA]);
        //         close(1);
        //          error = open(cmdsA[targetFileA], O_WRONLY|O_CREAT, 0644);
        //         if(error < 1)
        //             printf("Something went wrong in I/O indirection!\n");
        //         //dup2(error, 0);
        //         break;
        //     case APPEND:
        //       //  printf("Opening for append! targetFile=%s\n", cmdsA[targetFileA]);
        //         close(1);
        //         error = open(cmdsA[targetFileA], O_WRONLY|O_APPEND|O_CREAT, 0644);
        //         if(error < 0)
        //             printf("Something went wrong in I/O indirection!\n");
        //         //dup2(error, 1);
        //         break;
        //     default: 
        //         break;
        // }

        error = 0;
       //int x = execvp(cmdsA[0], cmdsA);
       //printf("Executing A!\n");

            int x = execve(pathA, cmdsA, __environ);        //close(error);
            if(x == -1)
                printf("ERROR A!\n");
    }
    else
    {
        close(pd[1]);
        close(0);
        dup(pd[0]);
        close(pd[0]);

        switch(indirectionB)
        {
            case 0:
                break;
            case INFILE:
              //  printf("Opening for input! targetFile=%s\n", cmdsB[targetFileB]);
                close(0);
                error = open(cmdsB[targetFileB], O_RDONLY);
                if(error < 0)
                    printf("Something went wrong in I/O indirection!\n");
                //dup2(error, 0);
                break;
            case OUTFILE:
             //   printf("Opening for output! targetFile=%s\n", cmdsB[targetFileB]);
                close(1);
                 error = open(cmdsB[targetFileB], O_WRONLY|O_CREAT, 0644);
                if(error < 1)
                    printf("Something went wrong in I/O indirection!\n");
                //dup2(error, 0);
                break;
            case APPEND:
             //   printf("Opening for append! targetFile=%s\n", cmdsB[targetFileB]);
                close(1);
                error = open(cmdsB[targetFileB], O_WRONLY|O_APPEND|O_CREAT, 0644);
                if(error < 0)
                    printf("Something went wrong in I/O indirection!\n");
                //dup2(error, 1);
                break;
            default: 
                break;
        }
        
        error = 0;
     
       
            int x = execve(pathB, cmdsB, __environ);
            //close(error);
            if(x == -1)
                printf("ERROR B!\n");
        
        
    }


	wait(&statusA);
    
	wait(&statusB);
    printf("Command 1 Exit: %d, Command 2 Exit: %d\n", statusA, statusB); // print exit status codes

}