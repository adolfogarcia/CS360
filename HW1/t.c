/************* t.c file ********************/
#include <stdio.h>
#include <stdlib.h>
#include "myprintf.c"

void myprintf(char *fmt,...);
int *FP;

int main(int argc, char *argv[ ], char *env[ ])
{
    int a,b,c;
    printf("enter main\n");

    myprintf("&argc=%p argv=%p env=%p\n", &argc, argv, env);
    myprintf("&a=%8p &b=%8p &c=%8p\n", &a, &b, &c);

//(1). Write C code to print values of argc and argv[] entries - DONE
    myprintf("argc = %d \n", argc);

    for(int i = 0; i < argc; i++)
    {
        myprintf("#%d) %s\n", i, argv[i]);
    }

    a=1; b=2; c=3;
    A(a,b);

    runTest();

    myprintf("exit main\n");

    
    
}


int A(int x, int y)
{
  int d,e,f;
  myprintf("enter A\n");
  // PRINT ADDRESS OF d, e, f
  myprintf("&d=%p &e=%p &f=%p\n", &d, &e, &f);

  d=4; e=5; f=6;
  B(d,e);
  myprintf("exit A\n");
}


int B(int x, int y)
{
  int g,h,i;
  myprintf("enter B\n");
  // PRINT ADDRESS OF g,h,i
  myprintf("&g=%p &h=%p &i=%p\n", &g, &h, &i);
  g=7; h=8; i=9;
  C(g,h);
  myprintf("exit B\n");
}

int C(int x, int y)
{
  int u, v, w, i, *p;

  myprintf("enter C\n");
  // PRINT ADDRESS OF u,v,w,i,p;
  myprintf("&u=%p &v=%p &w=%p &i=%p &p=%p\n", &u, &v, &w, &i, &p);
  u=10; v=11; w=12; i=13;

  FP =  (int *)  getebp();

//(2). Write C code to print the stack frame link list.

  myprintf("-------  PRINTING STACK FRAME LINK LINK -------------\n");
  while(FP)
  {
      myprintf("FP=%x  *FP=%d\n", FP, *FP);
      FP = (int*) *FP;
  }  
  myprintf("\n");

 p = (int *)&p;

//(3). Print the stack contents from p to the frame of main()
//     YOU MAY JUST PRINT 128 entries of the stack contents.
  myprintf("------------ PRINTING FROM P TO MAIN() --------------\n");
  for(int count = 0; count < 128 && p != FP; count++)
  {
    myprintf("p=%x   *p=%x\n", p, *p);
      
    p++;
  }  
  myprintf("\n");

//(4). On a hard copy of the print out, identify the stack contents
//     as LOCAL VARIABLES, PARAMETERS, stack frame pointer of each function.
}


//======================= Part 2 ========================================

//Given: putchar(char c) of Linux, which prints a char.

//2-1. Write YOUR own prints(char *s) fucntion to print a string.

void prints(char *s)
{
    for(int i = 0; s[i] != '\0'; i++)
    {
        putchar(s[i]);
    }
    putchar('\n');
}

//Given: The following printu() function prints an unsigned integer.

typedef unsigned int u32;

char *ctable = "0123456789ABCDEF";
int  BASE = 10; 

int rpu(u32 x, int base)
{  
    char c;
    if (x){
       c = ctable[x % base];
       rpu(x / base, base);
       putchar(c);
    }
}

int printu(u32 x, int base)
{
   (x==0)? putchar('0') : rpu(x, base);
   putchar(' ');
}

/*
 2-2. Write YOUR ONW fucntions 

 int  printd(int x) which prints an integer (x may be negative!!!)
 int  printx(u32 x) which prints x in HEX   (start with 0x )
 int  printo(u32 x) which prints x in Octal (start with 0  )
*/

int printd(int x)
{
    if(x < 0)
    {
        putchar('-');
    }

    printu(x, 10);
}

int printx(u32 x)
{
    myprintf("0x");

    printu(x, 16);
}

int printo(u32 x)
{
    putchar('0');
    printu(x, 8);
}

/********* PART 3 *********/

void myprintf(char* fmt, ...)
{
    int * ip = (int *) (&fmt + 1);

    for(int i = 0; fmt[i] != '\0'; i++)
    {
        if(fmt[i] != '%')
        {
            putchar(fmt[i]);
        }
        else if(fmt[i] == '%')
        {
            i++;
            switch(fmt[i])
            {
                case 'c':
                    putchar(*ip);
                    ip++;
                    break;
                case 's':
                    prints(*ip);
                    ip++;
                    break;
                case 'u':
                    printu(*ip, 10);
                    ip++;
                    break;
                case 'd':
                    printd(*ip);
                    ip++;
                    break;
                case 'o':
                    printo(*ip);
                    ip++;
                    break;
                case 'x':
                    printx(*ip);
                    ip++;
                    break;
                default:
                    break;
            }
        }
        else if(fmt[i] == '\n')
        {
            puts('\r\n');
        }
    }

}