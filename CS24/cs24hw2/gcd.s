#=============================================================================
# The gcd(a,b) function recursively computes gcd of two inputs
# a in rdi, b in rsi
.globl gcd
gcd:    
        cmpl    $0, %edi      # Checks if a is zero.
        jnz     gcd_recurse   # If not zero, goto recurse.
        movl    %esi, %eax    # If zero, move b into return address eax.
        jmp     gcd_return    # Jumps to return.
gcd_recurse:                
        movl    %esi, %eax    # Prepares register for division and
                              # move b into eax.
        cltd                  # Extend 0s into the register. 
        div     %edi          # Signed division of b / a , remainder in 
                              # rdx, quotient in rax.
        movl    %edi, %esi    # Copy a into argument location for b.
        movl    %edx, %edi    # Copy remainder into argument location for a.
        call    gcd           # Call gcd again on (b%a, a).
gcd_return:
        ret                   # return with answer in eax.
