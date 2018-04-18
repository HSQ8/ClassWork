/* This file contains x86-64 assembly-language implementations of three
 * basic, very common math operations.
 */

        .text

/*====================================================================
 * int f1(int x, int y)
 * x in edi, y in esi
 *  this function determins if two inputs are the same, if it is
 * then the input x is returned, else return y.
 */
.globl f1
f1:
        movl    %edi, %edx ; copy x into edx
        movl    %esi, %eax ; copy y into eax
        cmpl    %edx, %eax ; compare edx and eax 
        cmovg   %edx, %eax ; if equal, copy edx into eax
        ret                ; pop the base pointer


/*====================================================================
 * int f2(int x)
 *  takes int x, and return 1 if x is even and positive or odd and 
 * negative, 0 otherwise.
 */
.globl f2
f2:
        movl    %edi, %eax  ; move x into eax
        movl    %eax, %edx  ; copy eax into edx
        sarl    $31, %edx   ; shift edx right 31 bits
        xorl    %edx, %eax  ; perform bit xor on edx, eax store in eax
        subl    %edx, %eax  ; subtract eax from edx and store in eax
        ret      ; pop basepointer and return eaxre


/*====================================================================
 * int f3(int x)
 */
.globl f3
f3:
        movl    %edi, %edx ; move x to edx
        movl    %edx, %eax ; copy x in edx to eax
        sarl    $31, %eax  ; bit shift arithmetic x in eax 31 right
        testl   %edx, %edx ; logical ands edx and edx, checking whether
                           ; edx is above zero, zero, or below zero
        movl    $1, %edx
        cmovg   %edx, %eax
        ret

