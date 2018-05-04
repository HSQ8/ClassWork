my_setjmp:
    movq %rbp, (%rdi)
    movq %rbx, 8(%rdi)
    movq %r12, 16(%rdi)
    movq %r13, 24(%rdi)
    movq %r14, 32(%rdi)
    movq %r15, 40(%rdi)
    movq %rsp, 48(%rdi)
    movq (%rsp), %r11 
    movq %r11, 56(%rdi)
    movq $0, %rax
    ret

my_longjmp:
    movq (%rdi), %rbp
    movq 8(%rdi), %rbx
    movq 16(%rdi), %r12 
    movq 24(%rdi), %r13 
    movq 32(%rdi), %r14 
    movq 40(%rdi), %r15 
    movq 48(%rdi), %rsp 
    movq 56(%rdi), %r11
    movq %r11, (%rsp) 
    movq %rsi, %rax
    cmp  $0,   %rax
    jnz return_n 
    movq $1,  %rax  
return_n:
    ret


