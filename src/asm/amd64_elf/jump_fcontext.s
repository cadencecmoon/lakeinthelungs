.text
.globl jump_fcontext
.type jump_fcontext,@function
.align 16

jump_fcontext:
    pushq   %rbp /* save RBP */
    pushq   %rbx /* save RBX */
    pushq   %r15 /* save R15 */
    pushq   %r14 /* save R14 */
    pushq   %r13 /* save R13 */
    pushq   %r12 /* save R12 */

    /* prepare stack for FPU */
    leaq    -0x8(%rsp), %rsp

    /* test for flag preserve_fpu */
    cmp     $0, %rcx
    je      1f

    /* save MMX control-word and status-word */
    stmxcsr (%rsp)

    /* save x87 control-word */
    fnstcw  0x4(%rsp)

1:
    /* store RSP (pointing to context-data) in RDI */
    movq    %rsp, (%rdi)

    /* restore RSP (pointing to context-data) from RSI */
    movq    %rsi, %rsp

    /* test for flag preserve_fpu */
    cmp     $0, %rcx
    je      2f

    /* restore MMX control-word and status-word */
    ldmxcsr (%rsp)

    /* restore x87 control-word */
    fldcw   0x4(%rsp)

2:
    /* prepare stack for FPU */
    leaq    0x8(%rsp), %rsp

    popq    %r12 /* restore R12 */
    popq    %r13 /* restore R13 */
    popq    %r14 /* restore R14 */
    popq    %r15 /* restore R15 */
    popq    %rbx /* restore RBX */
    popq    %rbp /* restore RBP */

    /* restore return-address */
    popq    %r8

    /* use third arg as return-value after jump */
    movq    %rdx, %rax

    /* use third arg as first arg in context function */
    movq    %rdx, %rdi

    /* indirect jump to context */
    jmp     *%r8
.size jump_fcontext,.-jump_fcontext

/* mark that we don't need executable stack */
.section .note.GNU-stack,"",%progbits
