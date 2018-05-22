.text
.global output

output:
    movq $1, %rax       # move syscall write number into rax to invoke write
    movq %rsi, %rdx     # move number of characters into rdx
    movq %rdi, %rsi     # move the buffer pointer to rsi, 2nd argument of write
    movq $1, %rdi       # move constant 1, stdout into rdi
    syscall             # syscall
    ret                 # return
