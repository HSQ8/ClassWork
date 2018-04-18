#=============================================================================
# The gcd(a,b) function recursively computes gcd of two inputs
# a in rdi, b in rsi
.globl gcd
gcd:    
        cmpl    $0, edi     # check if a is zero
        jnz     gcd_recurse  # if not zero, goto recurse
        movl   %esi, eax    # if zero, move b into return address eax
        jmp     gcd_return  # jump to return
gcd_recurse:                
        divl   esi, edi     # signed division of b / a , remainder in rdx, quotient in rax
        movl   edi, esi     # copy a into argument location for b
        movl   rdx, edi     # copy remainder into argument location for a
        call gcd            # call gcd again on (b%a, a)
gcd_return:
        ret                 # return with answer in eax
