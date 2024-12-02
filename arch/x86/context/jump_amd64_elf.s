/*  Lake in the Lungs
 *  Copyright (c) 2024 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

/* TODO */

.intel_syntax noprefix
.text
.globl jump_fcontext
.type jump_fcontext,@function
.align 16

jump_fcontext:
    push    rbp /* save RBP */
    push    rbx /* save RBX */
    push    r15 /* save R15 */
    push    r14 /* save R14 */
    push    r13 /* save R13 */
    push    r12 /* save R12 */

    /* prepare stack for FPU */
    lea     rsp, [rsp-8h]

    /* test for flag preserve_fpu */
    test    rcx, rcx /* cmp rcx, 0h */
    je      1f

    /* save MMX control-word and status-word */
    stmxcsr [rsp]

    /* save x87 control-word */
    fnstcw  [rsp+4h]

1:
    /* store RSP (pointing to context-data) in RDI */
    mov     [rdi], rsp

    /* restore RSP (pointing to context-data) from RSI */
    mov     rsp, rsi

    /* test for flag preserve_fpu */
    test    rcx, rcx /* cmp rcx, 0h */
    je      2f

    /* restore MMX control-word and status-word */
    ldmxcsr [rsp]

    /* restore x87 control-word */
    fldcw   [rsp+4h]

2:
    /* prepare stack for FPU */
    lea     rsp, [rsp+8h]

    pop     r12 /* restore R12 */
    pop     r13 /* restore R13 */
    pop     r14 /* restore R14 */
    pop     r15 /* restore R15 */
    pop     rbx /* restore RBX */
    pop     rbp /* restore RBP */

    /* restore return-address */
    pop     r8

    /* use third arg as return-value after jump */
    mov     rax, rdx

    /* use third arg as first arg in context function */
    mov     rdi, rdx

    /* indirect jump to context */
    jmp     r8
.size jump_fcontext,.-jump_fcontext

/* mark that we don't need executable stack */
.section .note.GNU-stack,"",%progbits
