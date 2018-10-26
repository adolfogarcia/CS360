//#include "t.c"

int runTest()
{
    myprintf("\n\nTESTING myprintf() !!! \n\n");

    myprintf("Printing a hex value (345). Expected: 0x159 Actual: ");
    myprintf("%x\n", 345);
    myprintf("Printing a octal value (345). Expected: 0531 Actual: ");
    myprintf("%o\n", 345);
    myprintf("Printing an un_int value (345). Expected: 345 Actual: ");
    myprintf("%u\n", 345);
    myprintf("Printing a char value ('a'). Expected: a Actual: ");
    myprintf("%c\n", 'a');
    myprintf("Printing a decimal value (345). Expected: 345 Actual: ");
    myprintf("%d\n", 345);
    myprintf("Printing a string value. Expected: 'CS 360 is fun!' Actual: ");
    myprintf("%s\n", "CS 360 is fun!");
}