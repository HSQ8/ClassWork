#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include "gcd.h"

int error(const char* _msg){
    printf("%s\n", _msg);
    return 1;
}
int main(int argc, char** argv){
    char *ptr;
    int arg1, arg2;
    if( argc != 3){
        exit(error("incorrect number of arguments, requires 1"));
    }
    arg1 = strtol(argv[1],&ptr, 10);
    if(*ptr != '\0'){
        exit(error("not number"));
    }
    if(arg1 < 0){
        exit(error("negative number detected"));
    }
    arg2 = strtol(argv[2],&ptr, 10);
    if(*ptr != '\0'){
        exit(error("not number"));
    }
    if(arg2 < 0){
        exit(error("negative number detected"));
    }
    
    arg1 = gcd(arg1, arg2);
    printf("%d\n", arg1);
    return 0;
}