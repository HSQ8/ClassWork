#include <stdio.h>
#include <stdlib.h>
#include "gcd.h"

/**
 * Error function is responsible for printing error message and returning
 * Error argument.
 * @param  _msg A string of messages.
 * @return      1 for irregular exit.
 */
int error(const char* _msg)
{
    printf("%s\n", _msg);
    return 1;
}
/**
 * Takes two integer arguments and computes their gcd by the euclid
 * algorithm.
 * @param  argc [Number of arguments passed into the machine.]
 * @param  argv [The actual numeric arguments passed in from command line.]
 * @return      [0 if program exits correctly]
 */
int main(int argc, char** argv)
{
    char *ptr;
    int arg1, arg2;
    // checks for the correct number of arguments
    if( argc != 3)
    {
        exit(error("incorrect number of arguments, requires 2"));
    }
    // reads the first argument and enforces conditions like positive, integer
    arg1 = strtol(argv[1],&ptr, 10);
    if(*ptr != '\0')
    {
        exit(error("not number"));
    }
    if(arg1 < 0)
    {
        exit(error("negative number detected"));
    }
    // reads the second argument and enforces positive integer
    arg2 = strtol(argv[2],&ptr, 10);
    if(*ptr != '\0')
    {
        exit(error("not number"));
    }
    if(arg2 < 0)
    {
        exit(error("negative number detected"));
    }
    
    arg1 = gcd(arg1, arg2);
    printf("%d\n", arg1);
    return 0;
}