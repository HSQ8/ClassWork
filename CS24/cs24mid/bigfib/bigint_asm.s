# __add_bigint performs addition on Bigint type,
# which is an array of 64bit unsigned integers
# it treats each element of the array as the
# groups of 8 bytes, and accumulates the first
# input into the second input bigint. 
# this function carries over each segment of 
# addition to ensure correct results
# 
# Computes b += a
# 
# Arguments
# 
# rdi a - the pointer to the first bigint
# rsi b - the pointer to the second biging
# rdx size - the length of the array representation of bigint
# 
# Return
#
# will return zero if carry, 1 if no overflow or size = 0

.text
.global __add_bigint

__add_bigint:
    cmp  $0, %rdx              # checks whether we a have a nonzero size
    jz   RETURN_NO_CARRY
    movq $0, %r8               # initialize offset variable
    clc                        # clear carry flag

ADD_LOOP:
    movq (%rdi, %r8, 8), %rcx  # move a[i] into a temporary element
    adc  %rcx, (%rsi, %r8, 8)  # accumulate a[i] into b[i]
    inc  %r8                   # increment the counter variable
    dec  %rdx                  # decrement the loop variable
    jnz  ADD_LOOP              # exit loop variable if we reach the end
    jnc  RETURN_NO_CARRY       # if we don't overflow, goto normal return

RETURN_CARRY:                  # irregular return, overflow detected, 
    movq $0, %rax              # return 1
    ret

RETURN_NO_CARRY:               # normal return, no overflow, return 1
    movq $1, %rax              # move 0 into the return register
    ret                        # return 0


