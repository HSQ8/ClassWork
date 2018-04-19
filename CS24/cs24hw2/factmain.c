#include <stdio.h>
#include <stdlib.h>
#include "fact.h"
/**
 * Error function is responsible for printing error message and returning
 * Error argument.
 * @param  _msg A string of messages.
 * @return      1 for irregular exit.
 */
int error(const char* _msg){
	printf("%s\n", _msg);
	return 1;
}
/**
 * Main takes 1 command line argument integer and computes the factorial
 * of said integer.
 * @param  argc Number of arguments, should be 2.
 * @param  argv The first argument is program name, the second argument
 *              is the number for which we will compute the factorial.
 * @return      Return 0 for normal execution.
 */
int main(int argc, char** argv){
	char *ptr;
   	long long arg;
    // Checks for correct number of arguments
	if( argc != 2){
		exit(error("incorrect number of arguments, requires 1"));
	}
    // Attempts to read from input, enforces positive integer
	arg = strtol(argv[1],&ptr, 10);
	if(*ptr != '\0'){
		exit(error("not number"));
	}
	if(arg < 0){
		exit(error("negative number detected"));
	}
	arg = fact(arg);
	printf("%lld\n", arg);
	return 0;
}