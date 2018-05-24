.text
.global output

/**
 * outputs data to standard out.
 * output takes a pointer to a data buffer and a size and 
 * prepares a syscall to write by moving the argument passed in into the 
 * appropriate registers.
 */
output:
    movq $1, %rax    # move syscall write number (1) into rax to invoke write()
    movq %rsi, %rdx  # move number of characters into rdx, 3rd argument 
                     # of write
    movq %rdi, %rsi  # move the buffer pointer to rsi, 2nd argument of write
    movq $1, %rdi    # move constant 1 for file descriptor argument, meaning 
                     # we use standard output, into first argument of write
    syscall          # syscall, traps into kernel and invoke write using 
                     # arguments passed in.
    ret              # return number of bytes written in rax
