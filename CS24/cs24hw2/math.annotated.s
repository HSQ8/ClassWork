/* This file contains x86-64 assembly-language implementations of three
 * basic, very common math operations.
 */

        .text

/*====================================================================
 * int f1(int x, int y)
 * x in edi, y in esi
 *  this function returns the larger number
 */
.globl f1
f1:
        movl    %edi, %edx ; This copies x into edx
        movl    %esi, %eax ; This copies y into eax
        cmpl    %edx, %eax ; This compare edx and eax by performing subtraction
                           ;  and updating the flags appropriately 
        cmovg   %edx, %eax ; if x > y, copy edx(x) into eax, else return
                           ; y in eax
        ret               


/*====================================================================
 * int f2(int x)
 *  takes int x and returns absolute value of x
 */
.globl f2
f2:
        movl    %edi, %eax  ; Moves x into eax.
        movl    %eax, %edx  ; This line copies eax into edx.
        sarl    $31, %edx   ; This shifts edx right 31 bits to extract sign bit
        xorl    %edx, %eax  ; Perform bit xor on edx and eax and store in eax
        subl    %edx, %eax  ; Subtract eax from edx and store in eax
        ret     


/*====================================================================
 * int f3(int x)
 * returns the sign of x
 */
.globl f3
f3:
        movl    %edi, %edx ; Move x to edx.
        movl    %edx, %eax ; Copy x in edx to eax
        sarl    $31, %eax  ; Bit shift arithmetic x in eax 31 right
        testl   %edx, %edx ; Logical ands edx and edx and sets the flags
                           ; This results either in 0 or 1 in the zero
        movl    $1, %edx   ; Move 1 into edx.
        cmovg   %edx, %eax ; If the flag is 1, move edx into eax, 
                           ; Otherwise its zero.
        ret

