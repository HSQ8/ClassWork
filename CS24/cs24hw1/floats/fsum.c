#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "ffunc.h"


/* This function takes an array of single-precision floating point values,
 * and computes a sum in the order of the inputs.  Very simple.
 */
float fsum(FloatArray *floats) {
    float sum = 0;
    int i;

    for (i = 0; i < floats->count; i++)
        sum += floats->values[i];

    return sum;
}


/**
 * computes an accurate sum of a list of floating point values by
 * recursively dividing the list into two smaller lists and summing each individual result together
 *         
 * @param  floats [a pointer to a float array object that stores number of floats and also an array of
 * such floats]
 * @return        [the sum of the floats array as a single float]
 *
 * in the  simple base case, when the number of floats is less than a limit, we use simple sum
 * over the array to come up with the sum
 * when the array is larger than the predetermined limit, we split the array by creating two new
 * FloatArray objects.
 */
float my_fsum(FloatArray *floats) {
    FloatArray *lower, *upper;
    float s;
    int counter, limit = 2, size = floats->count;
    
    if(size < limit){
        s = floats->values[0];
        for(counter = 1; counter < size; ++counter){
            s += floats->values[counter];
        }
        return s;
    }else{
        lower = (FloatArray*) calloc(1, sizeof(FloatArray));
        upper = (FloatArray*) calloc(1, sizeof(FloatArray));
        lower->count = size/2;
        upper->count = size - size / 2;
        lower->values = (float*) calloc(size/2, sizeof(float));
        upper->values = (float*) calloc(size - size / 2, sizeof(float));

        for(counter = 0; counter < size/2; ++counter){
            lower->values[counter] = floats->values[counter];           
        }
        for(counter = size/2; counter < size; ++counter){
            upper->values[counter - size / 2] = floats->values[counter];
        }
        s = my_fsum(lower) + my_fsum(upper);
        free(lower->values);
        free(upper->values);
        free(lower);
        free(upper);
        return s;
    }
}


int main() {
    FloatArray floats;
    float sum1, sum2, sum3, my_sum;

    load_floats(stdin, &floats);
    printf("Loaded %d floats from stdin.\n", floats.count);

    /* Compute a sum, in the order of input. */
    sum1 = fsum(&floats);

    /* Use my_fsum() to compute a sum of the values.  Ideally, your
     * summation function won't be affected by the order of the input floats.
     */
    my_sum = my_fsum(&floats);

    /* Compute a sum, in order of increasing magnitude. */
    sort_incmag(&floats);
    sum2 = fsum(&floats);

    /* Compute a sum, in order of decreasing magnitude. */
    sort_decmag(&floats);
    sum3 = fsum(&floats);

    /* %e prints the floating-point value in full precision,
     * using scientific notation.
     */
    printf("Sum computed in order of input:  %e\n", sum1);
    printf("Sum computed in order of increasing magnitude:  %e\n", sum2);
    printf("Sum computed in order of decreasing magnitude:  %e\n", sum3);

    /* TODO:  UNCOMMENT WHEN READY!*/
    printf("My sum:  %e\n", my_sum);
    

    return 0;
}

