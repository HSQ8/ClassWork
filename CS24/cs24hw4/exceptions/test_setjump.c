// test for local variable change / stack corruption
// exception propagation across multiple function calls
// proper return, 1 or n
// exception within a simple function


#include "test_setjump.h"

static jmp_buf mybuffer;
static int test_count = 100;
int main(void) {
    srand(time(0));
    test_stack_corruption();
}  

void test_stack_corruption() {
    int upper = 100, lower = 100, passed = 1, j;
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
        if (test_simple_jump(a,b,c,d,e,f,g,h,i) != answer) {
            passed = 0;
        }
    }
    if (passed) {
        printf("%s\n", "executing stack corruption    PASSED");    
    } else {
        printf("%s\n", "executing stack corruption    FAILED"); 
    }
} 

int test_simple_jump(int a, int b, int c, int d, int e, int f, 
    int g, int h, int i) {
    int exception = my_setjmp(mybuffer);
    if (!exception) {
        fake_function();
    }
    return a + b + c + d + e + f + g + h + i;
}
void fake_function(){
    my_longjmp(mybuffer,0);
    printf("%s\n", "didn't jump");
    while (1){

    }
}