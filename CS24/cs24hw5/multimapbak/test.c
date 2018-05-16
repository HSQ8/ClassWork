#include <stdlib.h>
#include <stdio.h>
int main(void) {
    int* p = malloc(5*sizeof(int));
    int i;
    for (i = 0; i <5; ++i) {
        *(p + i) = i;
    }
    for (i = 0; i < 5; ++i) {
        printf("%d\n", *(p + i));
    }
    int* newp = realloc(p, 10 * sizeof(int));
    if (newp != NULL) {
        p = newp;
        for (i = 0; i <10; ++i) {
            *(p + i) = i;
        }
        for (i = 0; i < 10; ++i) {
            printf("%d\n", *(p + i));
        }
        free(p);
    }
    return 0;
}