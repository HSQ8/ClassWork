#include "my_setjmp.h"
/* Test for stack corruption*/
void test_stack_corruption();
/* helper function for testing for stack corruption*/
int test_simple_jump(int a, int b, int c, int d, int e, int f, 
    int g, int h, int i);
/* helper function for testing for stack corruption*/
void fake_function();

/* Test whether long jump can return the proper value passed to it*/
void test_longjmp_return ();
/* Helper function to testlongjump_return*/
int returnVal(int n);
/* Helper function to testlongjump_return*/
void returnValChild(int n);

/* Longjump return 1 test*/
void longjmp_ret_one(void);
/* long jump return 1 helper*/
void longjmp_ret_one_helper(void);

/* Tests for long jump's ability to propagate across many function calls*/
void longjump_across_functions(void);

/* Helper to propagation test*/
void longjump_across_functions_helper(void);
/* Helper to propagation test*/
void longjump_across_functions_helper2(void);
/* Helper to propagation test*/
void longjump_across_functions_helper3(void);