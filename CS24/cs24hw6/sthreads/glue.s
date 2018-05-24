# ============================================================================
# Keep a pointer to the main scheduler context.  This variable should be
# initialized to %rsp, which is done in the __sthread_start routine.
#
        .data
        .align 8
scheduler_context:      .quad   0


# ============================================================================
# __sthread_switch is the main entry point for the thread scheduler.
# It has three parts:
#
#    1. Save the context of the current thread on the stack.
#       The context includes all of the integer registers and RFLAGS.
#
#    2. Call __sthread_scheduler (the C scheduler function), passing the
#       context as an argument.  The scheduler stack *must* be restored by
#       setting %rsp to the scheduler_context before __sthread_scheduler is
#       called.
#
#    3. __sthread_scheduler will return the context of a new thread.
#       Restore the context, and return to the thread.
#
        .text
        .align 8
        .globl __sthread_switch
__sthread_switch:

        # Save the process state onto its stack
        push %rax  # push registers into the appropriate location in stack      
        push %rbx  # push registers into the appropriate location in stack     
        push %rcx  # push registers into the appropriate location in stack     
        push %rdx  # push registers into the appropriate location in stack     
        push %rsi  # push registers into the appropriate location in stack     
        push %rdi  # push registers into the appropriate location in stack     
        push %rbp  # push registers into the appropriate location in stack     
        push %r8   # push registers into the appropriate location in stack    
        push %r9   # push registers into the appropriate location in stack    
        push %r10  # push registers into the appropriate location in stack     
        push %r11  # push registers into the appropriate location in stack     
        push %r12  # push registers into the appropriate location in stack     
        push %r13  # push registers into the appropriate location in stack     
        push %r14  # push registers into the appropriate location in stack     
        push %r15  # push registers into the appropriate location in stack     
        pushf      # push flag register into appropriate location in stack

        # Call the high-level scheduler with the current context as an argument
        movq    %rsp, %rdi
        movq    scheduler_context, %rsp
        call    __sthread_scheduler

        # The scheduler will return a context to start.
        # Restore the context to resume the thread.
__sthread_restore:
        # Here, we pop everything into appropriate registers according
        # to the way we set it up in initialize
        movq %rax, %rsp    # adjust the stack pointer
        popf               # pop into flag registers
        pop %r15           # pop into the registers
        pop %r14           # pop into the registers
        pop %r13           # pop into the registers
        pop %r12           # pop into the registers
        pop %r11           # pop into the registers
        pop %r10           # pop into the registers
        pop %r9            # pop into the registers
        pop %r8            # pop into the registers
        pop %rbp           # pop into the registers
        pop %rdi           # pop into the registers
        pop %rsi           # pop into the registers
        pop %rdx           # pop into the registers
        pop %rcx           # pop into the registers
        pop %rbx           # pop into the registers
        pop %rax           # pop into the registers
        ret                # return by popping function address into rip


# ============================================================================
# Initialize a process context, given:
#    1. the stack for the process
#    2. the function to start
#    3. its argument
# The context should be consistent with that saved in the __sthread_switch
# routine.
#
# A pointer to the newly initialized context is returned to the caller.
# (This is the thread's stack-pointer after its context has been set up.)
#
# This function is described in more detail in sthread.c.
#
#
        .align 8
        .globl __sthread_initialize_context
__sthread_initialize_context:
        # the context is made up of three pieces, the first is 
        # the final thread finish function goes first so we can
        # ret and goto that function. We can then push the function
        # that we're threading so that when we first restore, we goto
        # the threading function. For every yield that we call, we would 
        # pushq the return address, except when we reach the end of the
        # thread, which there's no yield, then we ret to __sthread_finish.
        #
        # the following contains the registers in order. We save everything
        # except for rsp. here in saving the registers, we don't push, 
        # instead we use offsets of the stack pointer in rdi because if
        # we popped and pushed, we would be popping in the wrong place due
        # to the rsp not pointing to our own context. We use offsets here
        # because we also don't want to alter the rsp location.
        #
        # finally, we push 0 into every place corresponding to a register
        # since initially, we don't want any garbage values to potentially
        # interfere with our program
        #
        # We don't push 0 into the placeholder for rdi because when we restore
        # we need to populate that register with our input as part of our x64
        # calling convention for when we execute f.
        #
        # finally, we return the new context pointer by subtracting and
        # offset.
        movq $__sthread_finish, -8(%rdi) # push the final return function
                                         # address
        movq %rsi, -16(%rdi)             # push the function pointer f
        movq $0, -24(%rdi)               # insert place holder for %rax
        movq $0, -32(%rdi)               # insert place holder for %rbx
        movq $0, -40(%rdi)               # insert place holder for %rcx
        movq $0, -48(%rdi)               # insert place holder for %rdx
        movq $0, -56(%rdi)               # insert place holder for %rsi
        movq %rdx, -64(%rdi)             # insert place holder for %rdi
        movq $0, -72(%rdi)               # insert place holder for %rbp
        movq $0, -80(%rdi)               # insert place holder for %r8
        movq $0, -88(%rdi)               # insert place holder for %r9
        movq $0, -96(%rdi)               # insert place holder for %r10
        movq $0, -104(%rdi)              # insert place holder for %r11
        movq $0, -112(%rdi)              # insert place holder for %r12
        movq $0, -120(%rdi)              # insert place holder for %r13
        movq $0, -128(%rdi)              # insert place holder for %r14
        movq $0, -136(%rdi)              # insert place holder for %r15
        movq $0, -144(%rdi)              # placeholder for flag register
        movq %rdi, %rax                  # move context pointer
        sub $144, %rax                   # adjust the context pointer
        ret                              # return context pointer in rax


# ============================================================================
# The start routine initializes the scheduler_context variable, and calls
# the __sthread_scheduler with a NULL context.
#
# The scheduler will return a context to resume.
#
        .align 8
        .globl __sthread_start
__sthread_start:
        # Remember the context
        movq    %rsp, scheduler_context

        # Call the scheduler with no context
        movl    $0, %edi  # Also clears upper 4 bytes of %rdi
        call    __sthread_scheduler

        # Restore the context returned by the scheduler
        jmp     __sthread_restore

