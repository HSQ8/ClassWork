#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include "fact.h"

int error(const char* _msg){
	printf("%s\n", _msg);
	return 1;
}
int main(int argc, char** argv){
	char *ptr;
   	long long arg;
	if( argc != 2){
		exit(error("incorrect number of arguments, requires 1"));
	}
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