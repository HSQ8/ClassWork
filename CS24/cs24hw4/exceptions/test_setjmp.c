#include "test_setjmp.h"

static jmp_buf mybuffer;
static int test_count = 10;

/**
 * Main function that tests for four things
 * 1. Stack corruption of long jump.
 * 2. Test whether long jump can return values properly that are not  1.
 * 3. Test if long jump can return 1 if input is zero.
 * 4. Test if long jump can jump across functions.
 */
int main(void) {
    srand(time(0));
    test_stack_corruption();
    test_longjmp_return ();
    longjmp_ret_one();
    longjump_across_functions();
    return 0;
}  

/**
 * Test for stack corruption and whether the registers 
 * are begin saved or not by passing many arguments to the function
 * frame
 */
void test_stack_corruption() {
    int upper = 100, lower = 0, passed = 1, j;
    int a, b, c, d, e, f, g, h, i;
    for(int j = 0; j < test_count; ++j) {
        a = (rand() % (upper - lower + 1)) + lower;
        b = (rand() % (upper - lower + 1)) + lower;
        c = (rand() % (upper - lower + 1)) + lower;
        d = (rand() % (upper - lower + 1)) + lower;
        e = (rand() % (upper - lower + 1)) + lower;
        f = (rand() % (upper - lower + 1)) + lower;
        g = (rand() % (upper - lower + 1)) + lower;
        h = (rand() % (upper - lower + 1)) + lower;
        i = (rand() % (upper - lower + 1)) + lower;

        int answer = a + b + c + d + e + f + g + h + i;
        if (test_stack_corruption_helper(a,b,c,d,e,f,g,h,i) != answer) {
            passed = 0;
        }
    }
    if (passed) {
        printf("%s\n", "executing stack corruption    PASSED");    
    } else {
        printf("%s\n", "executing stack corruption    FAILED"); 
    }
} 


int test_stack_corruption_helper(int a, int b, int c, int d, int e, int f, 
    int g, int h, int i) {
    int exception = my_setjmp(mybuffer);
    if (!exception) {
        test_stack_corruption_helper_two();
    }
    return a + b + c + d + e + f + g + h + i;
}
void test_stack_corruption_helper_two(){
    my_longjmp(mybuffer, 0);
    printf("%s\n", "didn't jump");
}

/**
 * Test for long jump return value by passing in a random 
 * value 100 times and checking whether it matches the longjmp
 * return value. Does this with random numbers
 */
void test_longjmp_return () {
    int upper = 100, lower = 10, passed = 1, j;
    int a;
    for (int j = 0; j < test_count; ++j) {
        a = (rand() % (upper - lower + 1)) + lower;

        if (returnVal(a) != a) {
            passed = 0;
        }
    }
    if (passed == 1) {
        printf("%s\n", "executing long jump value passing    PASSED");    
    } else {
        printf("%s\n", "executing long jump value passing    FAILED"); 
    }
}

/**
 * helper function to long jump return that allows for a value to 
 * propagate back
 * @param  n the value we want to test
 * @return   the value returned by long jump
 */
int returnVal(int n) {
    int exception = my_setjmp(mybuffer);
    if (exception == 0) {
        returnValChild(n);
    }
    return exception;
}

void returnValChild(int n) {
    my_longjmp(mybuffer, n);
    printf("%s\n", "didn't jump");
}

/**
 * This functions tests in the case that long jump returns with a 
 * value passed in of 0, it should return 1.
 */
void longjmp_ret_one(void) {
    int n = my_setjmp(mybuffer);
    
    if (n == 0) { 
        longjmp_ret_one_helper();
    }

    if (n == 1) {
        printf("%s\n", "longjmp(buff,0) = 1    PASSED");    
    } else {
        printf("%s\n", "longjmp(buff,0) = 1    FAILED"); 
    }
}

/**
 * long jump return one helper function.
 */
void longjmp_ret_one_helper(void) {
    my_longjmp(mybuffer,0);
}

/**
 * This function tests for long jump's ability to propagate across
 * multiple functions. If it fails, then the function stack returns 
 * normally and we wouldn't get n set to 1. 
 */
void longjump_across_functions(void) {
    int n = my_setjmp(mybuffer);
    
    if (n == 0) { 
        longjump_across_functions_helper();
    }

    if (n == 1) {
        printf("%s\n", "long jump function propagation   PASSED");    
    } else {
        printf("%s\n", "long jump function propagation   FAILED"); 
    }
}

/**
 * helper function to long jump
 */
void longjump_across_functions_helper(void) {
    longjump_across_functions_helper2();
}

/**
 * helper function to long jump
 */
void longjump_across_functions_helper2(void) {
    longjump_across_functions_helper3();
}

/**
 * helper function to long jump
 */
void longjump_across_functions_helper3(void) {
    my_longjmp(mybuffer,0);
}

